//
//  MREViewController.h
//  ModelRenderEngine
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#import "MREMaterialsView.h"

@interface MREViewController : GLKViewController<MREMaterialsViewDataSource>

@property (retain, nonatomic) IBOutlet MREMaterialsView *materialsView;
@property (retain, nonatomic) IBOutlet UIToolbar *toolbar;

- (IBAction)onSelectFromLibrary:(id)sender;
- (IBAction)onSelectFromCamera:(id)sender;
- (IBAction)onSave:(id)sender;
- (IBAction)onCancel:(id)sender;

@end
