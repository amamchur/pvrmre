//
//  MREViewController.m
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREViewController.h"

#import "effect.h"
#import "model.h"

@interface MREViewController () {
    mre::model *model;
}

@property (strong, nonatomic) EAGLContext *context;

@end

@implementation MREViewController

- (void)dealloc {
    [self tearDownGL];
    [super dealloc];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    
    if (self.context == nil) {
        NSLog(@"Failed to create ES context");
    } else {    
        GLKView *view = (GLKView *)self.view;
        view.context = self.context;
        view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        
        UITapGestureRecognizer *gr = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
        [view addGestureRecognizer:gr];
        [gr release];
        
        [self setupGL];
        [self loadModel];
    }
}

- (void)viewDidUnload {
    [super viewDidUnload];    
    [self tearDownGL];
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

- (void)onTap:(UITapGestureRecognizer *)tap {
    GLKView *view = (GLKView *)self.view;
    CGRect rect = self.view.bounds;
    CGPoint point = [tap locationInView:view];
    if (model == NULL) {
        return;
    }
    int node = model->get_node_at_pos(point.x, point.y, rect.size.width, rect.size.height);
    model->set_selected_node(node);
    NSLog(@"Node selected %s", model->get_node_name(node).c_str());
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (model != NULL) {        
        model->setup(rect.size.width / rect.size.height);
        model->render();
    }
}

- (IBAction)onButton:(id)sender {
    if (model == NULL) {
        return;
    }
    
    int node = model->get_select_node();
    if (node >= 0) {
        GLuint textId = model->get_texture("textures/generics_epoca_senza_2.pvr");
        mre::effect_overrides eo;
        mre::texture_override to;
        to.unit = 0;
        to.textId = textId;
        eo.texture_overrides[0] = to;
        model->set_node_overrides(node, eo);
        model->set_selected_node(-1);
    }
}

@end
