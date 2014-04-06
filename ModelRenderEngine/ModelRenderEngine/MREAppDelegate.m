//
//  MREAppDelegate.m
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREAppDelegate.h"

#import "MREViewController.h"
#import "MREMenuViewController.h"

@implementation MREAppDelegate

- (void)dealloc {
    [_window release];
    [_viewController release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MREMenuViewController *menu = [[MREMenuViewController alloc] initWithNibName:@"MREMenuViewController"
                                                                          bundle:nil];
    UINavigationController *nc = [[UINavigationController alloc] initWithRootViewController:menu];
    nc.navigationBarHidden = YES;
        
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    self.viewController = nc;
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    
    [menu release];
    [nc release];    
    return YES;
}

@end
