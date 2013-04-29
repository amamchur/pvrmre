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

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell"];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
        cell.textLabel.textColor = [UIColor whiteColor];
        [cell autorelease];
    }
    cell.textLabel.text = @"Tap Me";
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    MREViewController *vc = [[[MREViewController alloc] initWithNibName:@"MREViewController" bundle:nil] autorelease];
    [self.navigationController pushViewController:vc animated:YES];
}

@end
