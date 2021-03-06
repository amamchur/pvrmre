//
//  MREViewController.m
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREViewController.h"
#import "MREMaterialsView.h"
#import "MREModelInfo.h"
#import "MRENodelInfo.h"
#import "MREEffectInfo.h"

#import "effect.h"
#import "model.h"
#import "texture_manager.h"

@interface MREViewController ()<
UIGestureRecognizerDelegate,
UIPopoverControllerDelegate,
UIImagePickerControllerDelegate,
UINavigationControllerDelegate> {
    mre::model *model;
    mre::texture_manager *textureManager;
    
    CPVRTBackground *background;
    GLuint backgroundTextId;
}

@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) NSTimer *hideToolbarTimer;
@property (strong, nonatomic) UIPopoverController *popover;
@property (copy, nonatomic) NSArray *activeMeterieals;

@end


@interface UIImage (fixOrientation)

- (UIImage *)fixOrientation;

@end

@implementation UIImage (fixOrientation)

- (UIImage *)fixOrientation {
    
    // No-op if the orientation is already correct
    if (self.imageOrientation == UIImageOrientationUp) return self;
    
    // We need to calculate the proper transformation to make the image upright.
    // We do it in 2 steps: Rotate if Left/Right/Down, and then flip if Mirrored.
    CGAffineTransform transform = CGAffineTransformIdentity;
    
    switch (self.imageOrientation) {
        case UIImageOrientationDown:
        case UIImageOrientationDownMirrored:
            transform = CGAffineTransformTranslate(transform, self.size.width, self.size.height);
            transform = CGAffineTransformRotate(transform, M_PI);
            break;
            
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
            transform = CGAffineTransformTranslate(transform, self.size.width, 0);
            transform = CGAffineTransformRotate(transform, M_PI_2);
            break;
            
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:
            transform = CGAffineTransformTranslate(transform, 0, self.size.height);
            transform = CGAffineTransformRotate(transform, -M_PI_2);
            break;
        case UIImageOrientationUp:
        case UIImageOrientationUpMirrored:
            break;
    }
    
    switch (self.imageOrientation) {
        case UIImageOrientationUpMirrored:
        case UIImageOrientationDownMirrored:
            transform = CGAffineTransformTranslate(transform, self.size.width, 0);
            transform = CGAffineTransformScale(transform, -1, 1);
            break;
            
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRightMirrored:
            transform = CGAffineTransformTranslate(transform, self.size.height, 0);
            transform = CGAffineTransformScale(transform, -1, 1);
            break;
        case UIImageOrientationUp:
        case UIImageOrientationDown:
        case UIImageOrientationLeft:
        case UIImageOrientationRight:
            break;
    }
    
    // Now we draw the underlying CGImage into a new context, applying the transform
    // calculated above.
    CGContextRef ctx = CGBitmapContextCreate(NULL, self.size.width, self.size.height,
                                             CGImageGetBitsPerComponent(self.CGImage), 0,
                                             CGImageGetColorSpace(self.CGImage),
                                             CGImageGetBitmapInfo(self.CGImage));
    CGContextConcatCTM(ctx, transform);
    switch (self.imageOrientation) {
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:
            // Grr...
            CGContextDrawImage(ctx, CGRectMake(0,0,self.size.height,self.size.width), self.CGImage);
            break;
            
        default:
            CGContextDrawImage(ctx, CGRectMake(0,0,self.size.width,self.size.height), self.CGImage);
            break;
    }
    
    // And now we just create a new UIImage from the drawing context
    CGImageRef cgimg = CGBitmapContextCreateImage(ctx);
    UIImage *img = [UIImage imageWithCGImage:cgimg];
    CGContextRelease(ctx);
    CGImageRelease(cgimg);
    return img;
}

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
    self.activeMeterieals = nil;
    self.materialsView = nil;
    self.toolbar = nil;
    self.popover = nil;
    [super dealloc];
}

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self != nil) {
        BOOL os7 = floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_6_1;
        if (os7) {
            self.automaticallyAdjustsScrollViewInsets = NO;
        }
    }
    return self;
}

- (MREMaterial *)materialWithEffect:(MREEffectInfo *)effect {
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:effect.thumbnail
                                                        ofType:@"jpg"
                                                   inDirectory:@"models/textures_thumb"];
    MREMaterial *m = [[MREMaterial alloc] init];
    m.image = [UIImage imageWithContentsOfFile:imgPath];
    m.effect = effect;
    return [m autorelease];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    self.preferredFramesPerSecond = 60;
    
    if (self.context == nil) {
        NSLog(@"Failed to create ES context");
        return;
    }
    
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    for (MRENodelInfo *info in _modelInfo.nodesInfo) {
        NSMutableArray *array = [NSMutableArray array];
        for (MREEffectInfo *effect in info.effects) {
            [array addObject:[self materialWithEffect:effect]];
        }
        [dict setObject:array forKey:info.nodeName];
    }
    self.materials = dict;
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
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
    [self resetCfg];
    
    background = new CPVRTBackground();
    background->Init(NULL, FALSE);
    backgroundTextId = 0;
    
    materialsView.contentInset = UIEdgeInsetsZero;
    materialsView.scrollIndicatorInsets = UIEdgeInsetsZero;
}

- (GLuint)loadTextureFromImage:(UIImage *)img {
    img = [img fixOrientation];
    if (img == nil) {
        return 0;
    }

    CGImageRef spriteImage = img.CGImage;
    GLsizei width = (GLsizei)CGImageGetWidth(spriteImage);
    GLsizei height = (GLsizei)CGImageGetHeight(spriteImage);
    
    GLubyte *data = (GLubyte *)malloc(width * height * 4 * sizeof(GLubyte));
    CGContextRef ctx = CGBitmapContextCreate(data,
                                             width,
                                             height,
                                             8,
                                             width*4,
                                             CGImageGetColorSpace(spriteImage),
                                             kCGImageAlphaPremultipliedLast);
    if (img.imageOrientation == UIImageOrientationDown) {
        CGContextRotateCTM(ctx, M_PI);
    }
    
    CGAffineTransform matrix = CGAffineTransformMake(1, 0, 0, -1, 0, height);
    CGContextConcatCTM(ctx, matrix);
    CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), spriteImage);
    CGContextRelease(ctx);
    
    GLuint texName;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    free(data);
    
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
        delete textureManager;
    }
    
    NSString *str = [[NSBundle mainBundle] bundlePath];
    NSString *dir = [str stringByAppendingPathComponent:@"models"];
    NSString *file = _modelInfo.file;

    model = new mre::model([dir UTF8String], [file UTF8String]);
    textureManager = new mre::texture_manager([dir UTF8String]);
    
    NSString *effects = [dir stringByAppendingPathComponent:@"mre.pfx"];
    CPVRTPFXParser parser;
    CPVRTString	errorStr;
    parser.ParseFromFile([effects UTF8String], &errorStr);
    
    model->load_effects(parser);
    model->camera.setup(model->model_box);
}

- (PVRTVec3)colorFromString:(NSString *)str {
    PVRTVec3 vec3;
    unsigned value = 0;
    [[NSScanner scannerWithString:str] scanHexInt:&value];
    vec3.x = ((value >> 16) & 0xFF) / 255.0f;
    vec3.y = ((value >> 8) & 0xFF) / 255.0f;
    vec3.z = ((value >> 0) & 0xFF) / 255.0f;
    return vec3;
}

- (mre::effect_overrides)overridesForEffect:(MREEffectInfo *)effect {
    mre::effect_overrides eo;
    int textureSlot = 0;
    for (NSString *texture in effect.textures) {
        NSString *path = [NSString stringWithFormat:@"textures/%@.pvr", texture];
        GLuint textId = textureManager->get_texture([path UTF8String]);
        mre::texture_override to;
        to.unit = textureSlot;
        to.textId = textId;
        eo.texture_overrides[textureSlot] = to;
        textureSlot++;
    }
    
    if (effect.ambient != nil) {
        PVRTVec3 vec3 = [self colorFromString:effect.diffuse];
        eo.uniform_overrides[ePVRTPFX_UsMATERIALCOLORAMBIENT] = vec3;
    }
    
    if (effect.diffuse != nil) {
        PVRTVec3 vec3 = [self colorFromString:effect.diffuse];
        eo.uniform_overrides[ePVRTPFX_UsMATERIALCOLORDIFFUSE] = vec3;
    }
    
    if (effect.specular != nil) {
        PVRTVec3 vec3 = [self colorFromString:effect.specular];
        eo.uniform_overrides[ePVRTPFX_UsMATERIALCOLORSPECULAR] = vec3;
    }
    
    return eo;
}

- (void)resetCfg {
    int count = model->get_node_count();
    for (int i = 0; i < count; i++) {
        std::string name = model->get_node_name(i);
        NSString *n = [NSString stringWithUTF8String:name.c_str()];
        NSArray *array = materials[n];
        if ([array count] == 0) {
            continue;
        }
        
        MREMaterial *m = array[0];
        MREEffectInfo *effect = m.effect;
        mre::effect_overrides eo = [self overridesForEffect:effect];
        model->set_node_overrides(i, eo);
        model->set_selected_node(-1);
    }
    
    model->set_light_index([_modelInfo.light intValue]);
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
    
    if (textureManager != NULL) {
        delete textureManager;
        textureManager = NULL;
    }
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    return touch.view == self.view;
}

- (void)onPinch:(UIPinchGestureRecognizer *)pinch {
    switch (pinch.state) {
        case UIGestureRecognizerStateBegan:
            pinch.scale = 1 / model->camera.distance;
            break;
        case UIGestureRecognizerStateChanged:
            model->camera.distance = 1 / pinch.scale;
            break;
        default:
            break;
    }
}

- (void)onPan:(UIPanGestureRecognizer *)pan {
    switch (pan.state) {
        case UIGestureRecognizerStateChanged: {
            CGPoint p  = [pan translationInView:self.view];
            model->camera.up_rotation -= p.x * 0.01;
            model->camera.right_rotation -= p.y * 0.01;
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
            PVRTVec3 v = model->camera.translation;
            v.x += p.x;
            v.y -= p.y;
            model->camera.translation = v;
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
        NSString *name = [NSString stringWithUTF8String:model->get_node_name(node).c_str()];
        self.activeMeterieals = [materials objectForKey:name];
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
    [materialsView setNeedsLayout];
    [materialsView layoutIfNeeded];
    
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
    return [_activeMeterieals count];
}

- (MREMaterial *)materialsView:(MREMaterialsView *)view materialAtIndex:(NSInteger)index {
    return [_activeMeterieals objectAtIndex:index];
}

- (void)materialsView:(MREMaterialsView *)view didSelectMaterialAtIndex:(NSInteger)index; {
    if (model == NULL) {
        [self hideMaterialView];
        return;
    }
    
    MREMaterial *m = [_activeMeterieals objectAtIndex:index];
    int node = model->get_select_node();
    if (node >= 0) {
        MREEffectInfo *effect = m.effect;
        mre::effect_overrides eo = [self overridesForEffect:effect];
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
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (backgroundTextId != 0) {
        background->Draw(backgroundTextId);
    }
    
    if (model != NULL) {        
        model->camera.aspect = rect.size.width / rect.size.height;
        model->camera.prepare();
        model->render();
    }
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
    BOOL os7 = floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_6_1;
    if (os7) {
        [picker dismissViewControllerAnimated:YES completion:NULL];
    } else {
        [[popover retain] autorelease];
        [popover dismissPopoverAnimated:YES];
        self.popover = nil;
    }
    [self hideToolbar];
    
    glDeleteTextures(1, &backgroundTextId);
    UIImage *img = [info objectForKey:UIImagePickerControllerOriginalImage];
    backgroundTextId = [self loadTextureFromImage:img];
    
    model->set_light_index([_modelInfo.light intValue]);
    model->camera.setup(model->model_box);
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
    BOOL os7 = floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_6_1;
    if (os7) {
        [self presentViewController:pc animated:YES completion:NULL];
    } else {
        UIPopoverController *p = [[UIPopoverController alloc] initWithContentViewController:pc];
        p.delegate = self;
        [p presentPopoverFromBarButtonItem:sender
                  permittedArrowDirections:UIPopoverArrowDirectionAny
                                  animated:YES];
        p.passthroughViews = [NSArray array];
        self.popover = p;
        [p release];
    }

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
