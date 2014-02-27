//
//  MREDataLoader.h
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 2/28/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void (^MRESuccessHandler)(id resource);
typedef void (^MREFailureHandler)(NSError *error);

@interface MREDataLoader : NSObject {
    MRESuccessHandler successHandler;
    MREFailureHandler failureHandler;
}

- (void)fetchUrl:(NSURL *)url
         rootCls:(Class)cls
         success:(MRESuccessHandler)success
         failure:(MREFailureHandler)failure;

@end
