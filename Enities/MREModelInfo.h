//
//  MREModelInfo.h
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 2/28/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MREModelInfo : NSObject

@property (copy, nonatomic) NSNumber *light;
@property (copy, nonatomic) NSString *title;
@property (copy, nonatomic) NSString *file;
@property (copy, nonatomic) NSArray *nodesInfo;

@end
