//
//  MREEffectInfo.h
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 4/7/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MREEffectInfo : NSObject

@property (copy, nonatomic) NSString *thumbnail;
@property (copy, nonatomic) NSArray *textures;
@property (copy, nonatomic) NSString *diffuse;
@property (copy, nonatomic) NSString *ambient;
@property (copy, nonatomic) NSString *specular;

@end
