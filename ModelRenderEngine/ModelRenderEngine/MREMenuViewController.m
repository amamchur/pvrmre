//
//  MREMenuViewController.m
//  ModelRenderEngine
//
//  Created by admin on 4/30/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREMenuViewController.h"
#import "MREViewController.h"

@interface MREMenuViewController ()

@end

@implementation MREMenuViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell"];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
        cell.textLabel.textColor = [UIColor whiteColor];
        cell.backgroundColor = [UIColor blackColor];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
        [cell autorelease];
    }
    
    switch (indexPath.row) {
        case 0:
            cell.textLabel.text = @"Chair S";
            break;
        case 1:
            cell.textLabel.text = @"Selva Vendome Bergere";
            break;
    }
    return cell;
}

- (MREMaterial *)materialWithName:(NSString *)name {
    NSString *imgPath = [[NSBundle mainBundle] pathForResource:name
                                                        ofType:@"jpg"
                                                   inDirectory:@"models/textures_thumb"];
    MREMaterial *m = [[MREMaterial alloc] init];
    m.image = [UIImage imageWithContentsOfFile:imgPath];
    m.texture = [NSString stringWithFormat:@"textures/%@.pvr", name];
    m.name = name;
    return [m autorelease];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NSDictionary *materials = nil;
    NSString *name = nil;
    switch (indexPath.row) {
        case 0:
            name = @"e142.pod";
            materials = @{
                          @"Object001" : @[
                                  [self materialWithName:@"st263"],
                                  [self materialWithName:@"st2090"]
                                  ],
                          @"Plane003": @[
                                  [self materialWithName:@"generics_epoca_senza"],
                                  [self materialWithName:@"w_1"],
                                  [self materialWithName:@"w_2"],
                                  [self materialWithName:@"w_3"],
                                  [self materialWithName:@"w_4"]
                                  ]
                          };
            break;
        case 1:
            name = @"selva_poltron.pod";
            materials = @{
                          @"fabrick": @[
                                  [self materialWithName:@"f_1_d"],
                                  [self materialWithName:@"f_2_d"],
                                  [self materialWithName:@"f_3_d"],
                                  [self materialWithName:@"f_4_d"],
                                  [self materialWithName:@"f_5_d"],
                                  [self materialWithName:@"f_6_d"]
                                  ],
                          @"wood" : @[
                                  [self materialWithName:@"generics_epoca_senza"],
                                  [self materialWithName:@"w_1"],
                                  [self materialWithName:@"w_2"],
                                  [self materialWithName:@"w_3"],
                                  [self materialWithName:@"w_4"]
                                  ]
                          };
            break;
    }

    MREViewController *vc = [[[MREViewController alloc] initWithNibName:@"MREViewController" bundle:nil] autorelease];
    vc.modelName = name;
    vc.materials = materials;
    [self.navigationController pushViewController:vc animated:YES];
}

@end
