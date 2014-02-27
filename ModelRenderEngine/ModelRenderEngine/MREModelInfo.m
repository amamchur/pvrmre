//
//  MREModelInfo.m
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 2/28/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import "MREModelInfo.h"
#import "MRENodelInfo.h"

#import "JsonLiteObjC/JsonLiteObjc.h"

@implementation MREModelInfo

- (void)dealloc {
    self.title = nil;
    self.file = nil;
    self.nodesInfo = nil;
    [super dealloc];
}

+ (NSArray *)jsonLiteBindingRules {
    return @[[JsonLiteBindRule ruleForKey:@"nodesInfo" elementClass:[MRENodelInfo class]]];
}

@end
