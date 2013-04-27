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
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setupGL];
    [self loadModel];
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
    
    NSString *file = [[NSBundle mainBundle] pathForResource:@"e142"
                                                     ofType:@"pod"
                                                inDirectory:@"models"];
    NSLog(@"%@", file);
    
    CPVRTModelPOD *pod = new CPVRTModelPOD();    
    EPVRTError error = pod->ReadFromFile([file UTF8String]);
    if (error == PVR_SUCCESS) {
        model = new mre::model(pod);
    }    
}

- (void)setupGL {
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL {
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update {
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (model != NULL) {
        model->render();
    }
    
}

@end
