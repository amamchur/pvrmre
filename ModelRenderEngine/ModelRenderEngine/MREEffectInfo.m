//
//  MREEffectInfo.m
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 4/7/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import "MREEffectInfo.h"

@implementation MREEffectInfo

- (void)dealloc {
    self.thumbnail = nil;
    self.textures = nil;
    self.diffuse = nil;
    self.ambient = nil;
    self.specular = nil;
    [super dealloc];
}

@end
