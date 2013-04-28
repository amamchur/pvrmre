//
//  MREViewController.m
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREViewController.h"

#import "model.h"

@interface MREViewController () {
    mre::model *model;
}

@property (strong, nonatomic) EAGLContext *context;

@end

@implementation MREViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    self.preferredFramesPerSecond = 60;
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    UITapGestureRecognizer *gr = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
    [view addGestureRecognizer:gr];
    [gr release];
    
    [self setupGL];
    [self loadModel];
}

- (void)onTap:(UITapGestureRecognizer *)tap {
    GLKView *view = (GLKView *)self.view;
    CGRect rect = self.view.bounds;
    CGPoint point = [tap locationInView:view];
    if (model != NULL) {
        int node = model->get_node_at_pos(point.x, point.y, rect.size.width, rect.size.height);
        model->set_selected_node(node);
    }
}

- (void)viewDidUnload {
    [super viewDidUnload];
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
    
    delete model;
    model = NULL;    
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
    [EAGLContext setCurrentContext:nil];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update {
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (model != NULL) {        
        model->setup(rect.size.width / rect.size.height);
        model->render();
    }
    
}

@end
