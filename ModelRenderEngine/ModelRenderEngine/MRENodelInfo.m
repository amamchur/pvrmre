//
//  MRENodelInfo.m
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 2/28/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import "MRENodelInfo.h"

@implementation MRENodelInfo

- (void)dealloc {
    self.nodeName = nil;
    self.materials = nil;
    [super dealloc];
}

@end
