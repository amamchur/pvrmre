//
//  MREViewController.m
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREViewController.h"
#import "MREMaterialsView.h"

#import "effect.h"
#import "model.h"

@interface MREViewController ()<
UIGestureRecognizerDelegate,
UIPopoverControllerDelegate,
UIImagePickerControllerDelegate,
UINavigationControllerDelegate> {
    mre::model *model;
    CPVRTBackground *background;
    GLuint backgroundTextId;
}

@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) NSMutableArray *materials;
@property (strong, nonatomic) NSTimer *hideToolbarTimer;
@property (strong, nonatomic) UIPopoverController *popover;

@end

@implementation MREViewController

@synthesize context;
@synthesize materials;
@synthesize toolbar;
@synthesize hideToolbarTimer;
@synthesize materialsView;
@synthesize popover;

- (void)dealloc {
    [self tearDownGL];
    
    [hideToolbarTimer invalidate];
    self.hideToolbarTimer = nil;
    
    self.materials = nil;
    self.materialsView = nil;
    self.toolbar = nil;
    self.popover = nil;
    [super dealloc];
}

- (MREMaterial *)materialWithName:(NSString *)name {
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:name
                                                        ofType:@"jpg"
                                                   inDirectory:@"models/textures_thumb"];
    MREMaterial *m = [[MREMaterial alloc] init];
    m.image = [UIImage imageWithContentsOfFile:imgPath];
    m.texture = [NSString stringWithFormat:@"textures/%@.pvr", name];
    m.name = name;
    return [m autorelease];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    self.preferredFramesPerSecond = 60;
    self.materials = [NSMutableArray array];
    
    [materials addObject:[self materialWithName:@"generics_epoca_senza"]];
    [materials addObject:[self materialWithName:@"st263"]];
    [materials addObject:[self materialWithName:@"wallpapers"]];
    [materials addObject:[self materialWithName:@"brickwork-texture"]];
    
    if (self.context == nil) {
        NSLog(@"Failed to create ES context");
        return;
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    UITapGestureRecognizer *gr = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
    gr.delegate = self;
    [view addGestureRecognizer:gr];
    [gr release];
    
    UIPanGestureRecognizer *pgr = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPan:)];
    pgr.minimumNumberOfTouches = 1;
    pgr.maximumNumberOfTouches = 1;
    pgr.delegate = self;
    [view addGestureRecognizer:pgr];
    [pgr release];
    
    UIPanGestureRecognizer *dpgr = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onDoublePan:)];
    dpgr.minimumNumberOfTouches = 2;
    dpgr.maximumNumberOfTouches = 2;
    dpgr.delegate = self;
    [view addGestureRecognizer:dpgr];
    [dpgr release];
    
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(onPinch:)];
    pinch.delegate = self;
    [view addGestureRecognizer:pinch];
    [pinch release];
    
    [self setupGL];
    [self loadModel];
    
    background = new CPVRTBackground();
    background->Init(NULL, FALSE);
    backgroundTextId = 0;
}

- (GLuint)loadTextureFromImage:(UIImage *)img {
    if (img == nil) {
        return 0;
    }
    
    CGImageRef spriteImage = img.CGImage;
    size_t width = CGImageGetWidth(spriteImage);
    size_t height = CGImageGetHeight(spriteImage);
    
    GLubyte * spriteData = (GLubyte *)calloc(width*height*4, sizeof(GLubyte));    
    CGContextRef ctx = CGBitmapContextCreate(spriteData,
                                             width,
                                             height,
                                             8,
                                             width*4,
                                             CGImageGetColorSpace(spriteImage),
                                             kCGImageAlphaPremultipliedLast);
    CGAffineTransform flipVertical = CGAffineTransformMake(1, 0, 0, -1, 0,height);
    CGContextConcatCTM(ctx, flipVertical);
    CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), spriteImage);
    CGContextRelease(ctx);
    
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
    
    free(spriteData);
    
    return texName;
}

- (void)viewDidUnload {
    [super viewDidUnload];    
    [self tearDownGL];
    
    [hideToolbarTimer invalidate];
    self.hideToolbarTimer = nil;
    
    self.materials = nil;
    self.materialsView = nil;
    self.toolbar = nil;
    self.popover = nil;
}

- (void)loadModel {
    if (model == NULL) {
        delete model;
    }
    
    NSString *str = [[NSBundle mainBundle] bundlePath];    
    model = new mre::model([[str stringByAppendingPathComponent:@"models"] UTF8String], "e142.pod");
}

- (void)setupGL {
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL {
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
    
    if (background != NULL) {
        delete background;
        background = NULL;
    }
    
    glDeleteTextures(1, &backgroundTextId);
    backgroundTextId = 0;
    
    if (model != NULL) {
        delete model;
        model = NULL;
    }
    
    delete model;
    model = NULL;
}

- (signed char)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    return touch.view == self.view;
}

- (void)onPinch:(UIPinchGestureRecognizer *)pinch {
    switch (pinch.state) {
        case UIGestureRecognizerStateBegan:
            pinch.scale = 1 / model->get_distance();
            break;
        case UIGestureRecognizerStateChanged:
            model->set_distance(1 / pinch.scale);
            break;
        default:
            break;
    }
}

- (void)onPan:(UIPanGestureRecognizer *)pan {
    switch (pan.state) {
        case UIGestureRecognizerStateChanged: {
            CGPoint p  = [pan translationInView:self.view];
            model->set_up_rotation(model->get_up_rotation() - p.x * 0.01);
            model->set_right_rotation(model->get_right_rotation() - p.y * 0.01);
            [pan setTranslation:CGPointZero inView:self.view];
            break;
        }
        default:
            break;
    }
}

- (void)onDoublePan:(UIPanGestureRecognizer *)pan {
    switch (pan.state) {
        case UIGestureRecognizerStateChanged: {
            CGPoint p  = [pan translationInView:self.view];
            PVRTVec3 v = model->get_viewport_translation();
            v.x += p.x;
            v.y -= p.y;
            model->set_viewport_translation(v);
            [pan setTranslation:CGPointZero inView:self.view];
            break;
        }
        default:
            break;
    }
}

- (void)onTap:(UITapGestureRecognizer *)tap {
    if (model == NULL) {
        return;
    }
    
    GLKView *view = (GLKView *)self.view;
    CGRect rect = self.view.bounds;
    CGPoint point = [tap locationInView:view];
    CGFloat scale = [[UIScreen mainScreen] scale];
    point.x *= scale;
    point.y *= scale;
    rect.size.width *= scale;
    rect.size.height *= scale;
    
    int node = model->get_node_at_pos(point.x, point.y, rect.size.width, rect.size.height);
    if (node >= 0) {
        [self showMaterialView];
        [self hideToolbar];
    } else {
        [self hideMaterialView];
        [self showToolbar];
    }
    model->set_selected_node(node);
}

#pragma mark - UICollectionView delegate methods

- (void)showToolbar {
    [UIView beginAnimations:nil context:NULL];
    CGRect rect = toolbar.frame;
    rect.origin.y = 0;
    toolbar.frame = rect;
    [UIView commitAnimations];
    
    [hideToolbarTimer invalidate];
    self.hideToolbarTimer = [NSTimer scheduledTimerWithTimeInterval:5
                                                    target:self
                                                  selector:@selector(hideToolbar)
                                                  userInfo:nil
                                                   repeats:NO];
}

- (void)hideToolbar {
    if (popover == nil) {
        [UIView beginAnimations:nil context:NULL];
        CGRect rect = toolbar.frame;
        rect.origin.y = -rect.size.height;
        toolbar.frame = rect;
        [UIView commitAnimations];
    }
    
    [hideToolbarTimer invalidate];
    [[hideToolbarTimer retain] autorelease];
    self.hideToolbarTimer = nil;
}

- (void)showMaterialView {
    [UIView beginAnimations:nil context:NULL];
    CGRect rect = materialsView.frame;
    rect.origin.y = self.view.bounds.size.height - rect.size.height;
    materialsView.frame = rect;
    [UIView commitAnimations];
}

- (void)hideMaterialView {
    [UIView beginAnimations:nil context:NULL];
    CGRect rect = materialsView.frame;
    rect.origin.y = self.view.bounds.size.height;
    materialsView.frame = rect;
    [UIView commitAnimations];
}

- (NSInteger)materialsViewNumberOfMeterial:(MREMaterialsView *)view {
    return [materials count];
}

- (MREMaterial *)materialsView:(MREMaterialsView *)view materialAtIndex:(NSInteger)index {
    return [materials objectAtIndex:index];
}

- (void)materialsView:(MREMaterialsView *)view didSelectMaterialAtIndex:(NSInteger)index; {
    if (model == NULL) {
        [self hideMaterialView];
        return;
    }
    
    MREMaterial *m = [materials objectAtIndex:index];
    int node = model->get_select_node();
    if (node >= 0) {
        GLuint textId = model->get_texture([m.texture UTF8String]);
        mre::effect_overrides eo;
        mre::texture_override to;
        to.unit = 0;
        to.textId = textId;
        eo.texture_overrides[0] = to;
        model->set_node_overrides(node, eo);
        model->set_selected_node(-1);
    }
    [self hideMaterialView];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (context == nil) {
        return;
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (backgroundTextId != 0) {
        background->Draw(backgroundTextId);
    }
    
    if (model != NULL) {        
        model->setup(rect.size.width / rect.size.height);
        model->render();
    }
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
    [[popover retain] autorelease];
    [popover dismissPopoverAnimated:YES];
    self.popover = nil;
    [self hideToolbar];
    
    glDeleteTextures(1, &backgroundTextId);
    UIImage *img = [info objectForKey:UIImagePickerControllerOriginalImage];
    backgroundTextId = [self loadTextureFromImage:img];
    
    model->setup_default_camera();
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController {
    [[popover retain] autorelease];
    self.popover = nil;
    [self hideToolbar];
}

- (IBAction)onSelectFromLibrary:(id)sender {
    UIImagePickerController *pc = [[UIImagePickerController alloc] init];
    pc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    pc.delegate = self;
    UIPopoverController *p = [[UIPopoverController alloc] initWithContentViewController:pc];
    p.delegate = self;
    [p presentPopoverFromBarButtonItem:sender
              permittedArrowDirections:UIPopoverArrowDirectionAny
                              animated:YES];
    p.passthroughViews = [NSArray array];
    self.popover = p;
    [p release];
    [pc release];
}

- (IBAction)onSelectFromCamera:(id)sender {
    UIImagePickerController *pc = [[UIImagePickerController alloc] init];
    pc.sourceType = UIImagePickerControllerSourceTypeCamera;
    pc.delegate = self;
    UIPopoverController *p = [[UIPopoverController alloc] initWithContentViewController:pc];
    p.delegate = self;
    [p presentPopoverFromBarButtonItem:sender
              permittedArrowDirections:UIPopoverArrowDirectionAny
                              animated:YES];
    p.passthroughViews = [NSArray array];
    self.popover = p;
    [p release];
    [pc release];
}

- (IBAction)onSave:(id)sender {
    GLKView *view = (GLKView *)self.view;
    UIImage *img = [view snapshot];
    UIImageWriteToSavedPhotosAlbum(img, NULL, NULL, NULL);
}

- (IBAction)onCancel:(id)sender {
    [self tearDownGL];
    [self.navigationController popViewControllerAnimated:YES];
}

@end
