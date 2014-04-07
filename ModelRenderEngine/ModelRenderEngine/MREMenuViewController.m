//
//  MREMenuViewController.m
//  ModelRenderEngine
//
//  Created by admin on 4/30/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREMenuViewController.h"
#import "MREViewController.h"

#import "MREDataLoader.h"
#import "MREModelInfo.h"
#import "MRENodelInfo.h"

#import "JsonLiteObjC/JsonLiteObjc.h"

@interface MREMenuViewController ()

@property (retain, nonatomic) IBOutlet UITableView *table;
@property (copy, nonatomic) NSArray *models;

@end

@implementation MREMenuViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)dealloc {
    self.table = nil;
    self.models = nil;
    [super dealloc];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"models"
                                                     ofType:@"json"
                                                   inDirectory:@"models"];
    NSData *data = [NSData dataWithContentsOfFile:path];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[MREModelInfo class]];
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:32];
    parser.delegate = deserializer;
    [parser parse:data];
    
    self.models = [deserializer object];
    [_table reloadData];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [_models count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    MREModelInfo *model = [_models objectAtIndex:indexPath.row];
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell"];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
        cell.textLabel.textColor = [UIColor whiteColor];
        cell.backgroundColor = [UIColor blackColor];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
        [cell autorelease];
    }
    
    cell.textLabel.text = model.title;
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    MREModelInfo *model = [_models objectAtIndex:indexPath.row];
    MREViewController *vc = [[[MREViewController alloc] initWithNibName:@"MREViewController" bundle:nil] autorelease];
    vc.modelInfo = model;
    [self.navigationController pushViewController:vc animated:YES];
}

@end
