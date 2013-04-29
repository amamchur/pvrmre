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

@interface MREViewController () {
    mre::model *model;
}

@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) NSMutableArray *materials;

@end

@implementation MREViewController

@synthesize context;
@synthesize materials;
@synthesize materialsView;

- (void)dealloc {
    [self tearDownGL];
    self.materials = nil;
    self.materialsView = nil;
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

    if (self.context == nil) {
        NSLog(@"Failed to create ES context");
    } else {    
        GLKView *view = (GLKView *)self.view;
        view.context = self.context;
        view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        
        UITapGestureRecognizer *gr = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
        [view addGestureRecognizer:gr];
        [gr release];
        
        UIPanGestureRecognizer *pgr = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPan:)];
        [view addGestureRecognizer:pgr];
        [pgr release];
        
        UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(onPinch:)];
        [view addGestureRecognizer:pinch];
        [pinch release];
        
        [self setupGL];
        [self loadModel];
    }
}

- (void)viewDidUnload {
    [super viewDidUnload];    
    [self tearDownGL];
    
    self.materials = nil;
    self.materialsView = nil;
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
    
    delete model;
    model = NULL;
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
    } else {
        [self hideMaterialView];
    }
    model->set_selected_node(node);
}

#pragma mark - UICollectionView delegate methods

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
    if (model != NULL) {        
        model->setup(rect.size.width / rect.size.height);
        model->render();
    }
}

@end
