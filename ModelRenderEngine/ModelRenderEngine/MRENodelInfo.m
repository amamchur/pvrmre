//
//  MRENodelInfo.m
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 2/28/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import "MRENodelInfo.h"
#import "MREEffectInfo.h"
#import "JsonLiteObjC/JsonLiteObjC.h"

@implementation MRENodelInfo

- (void)dealloc {
    self.nodeName = nil;
    self.effects = nil;
    [super dealloc];
}

+ (NSArray *)jsonLiteBindingRules {
    return @[
             [JsonLiteBindRule ruleForKey:@"effects" elementClass:[MREEffectInfo class]]
             ];
}


@end
