//
//  MREDataLoader.m
//  ModelRenderEngine
//
//  Created by Andrii Mamchur on 2/28/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import "MREDataLoader.h"
#import "JsonLiteObjC/JsonLiteObjc.h"

@interface MREDataLoader()<NSURLConnectionDataDelegate, NSURLConnectionDelegate>

@property (strong, nonatomic) NSURLConnection *connection;
@property (strong, nonatomic) JsonLiteParser *parser;
@property (strong, nonatomic) JsonLiteDeserializer *deserializer;
@property (strong, nonatomic) id selfOwnerShip;

@end

@implementation MREDataLoader

- (void)dealloc {
    self.connection = nil;
    self.parser = nil;
    self.deserializer = nil;
    self.selfOwnerShip = nil;
    Block_release(successHandler);
    Block_release(failureHandler);
    [super dealloc];
}

- (void)fetchUrl:(NSURL *)url
         rootCls:(Class)cls
         success:(MRESuccessHandler)success
         failure:(MREFailureHandler)failure {
    successHandler = Block_copy(success);
    failureHandler = Block_copy(failure);
    
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    self.deserializer = [JsonLiteDeserializer deserializerWithRootClass:cls];
    self.parser = [JsonLiteParser parserWithDepth:32];
    self.parser.delegate = _deserializer;
    self.connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
    self.selfOwnerShip = self;
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data; {
    [_parser parse:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection; {
    id obj = [_deserializer object];
    if (successHandler !=  NULL) {
        successHandler(obj);
    }
    
    self.selfOwnerShip = self;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    if (failureHandler !=  NULL) {
        failureHandler(error);
    }
    
    self.selfOwnerShip = self;
}

@end
