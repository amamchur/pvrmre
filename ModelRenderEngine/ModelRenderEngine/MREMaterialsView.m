//
//  MREMaterialsView.m
//  ModelRenderEngine
//
//  Created by admin on 4/29/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "MREMaterialsView.h"
#import <QuartzCore/QuartzCore.h>
#import <CoreGraphics/CoreGraphics.h>

#define IMAGE_VIEW_SIZE 110

@implementation MREMaterial

@synthesize image;
@synthesize name;
@synthesize texture;

- (void)dealloc {
    self.image = nil;
    self.name = nil;
    self.texture = nil;
    [super dealloc];
}

@end

@interface MREMaterialView()

@property (nonatomic, assign) NSInteger index;

@end

@implementation MREMaterialView

@synthesize imageView;
@synthesize label;

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self != nil) {
        imageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, IMAGE_VIEW_SIZE, IMAGE_VIEW_SIZE)];
        label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, IMAGE_VIEW_SIZE, IMAGE_VIEW_SIZE)];
        imageView.autoresizingMask = ~0;
        imageView.layer.shadowColor = [UIColor blackColor].CGColor;
        imageView.layer.shadowOpacity = 0.7;
        imageView.layer.shadowRadius = 5;
        imageView.layer.shadowOffset = CGSizeMake(3, 3);
        
        label.autoresizingMask = ~0;
        [self addSubview:imageView];
        [self addSubview:label];
        imageView.backgroundColor = [UIColor redColor];
        imageView.center = CGPointMake(128 / 2, 128 / 2);
        label.center = CGPointMake(128 / 2, 128 / 2);
        label.backgroundColor = [UIColor clearColor];
        [imageView release];
        [label release];
    }
    return self;
}

@end

@interface MREMaterialsView()

@property (nonatomic, retain) NSMutableArray *pool;

@end

@implementation MREMaterialsView

@synthesize datasource;
@synthesize delegate;

- (id)initWithCoder:(NSCoder *)aDecoder {
    self = [super initWithCoder:aDecoder];
    if (self != nil) {
        UITapGestureRecognizer *tgr = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                              action:@selector(onTap:)];
        [self addGestureRecognizer:tgr];
        [tgr release];
    }
    return self;
}

- (void)onTap:(UITapGestureRecognizer *)tgr {
    CGPoint p = [tgr locationInView:self];
    UIView *v = [self hitTest:p withEvent:nil];
    if ([v isKindOfClass:[MREMaterialView class]]) {
        MREMaterialView *mv = (MREMaterialView *)v;
        [delegate materialsView:self didSelectMaterialAtIndex:mv.index];
    }
}

- (NSArray *)visibleViews {
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:13];
    for (UIView *view in self.subviews) {
        if ([view isKindOfClass:[MREMaterialView class]]) {
            [array addObject:view];
        }
    }
    
    return array;
}

- (NSArray *)visibleViewsIndexes {
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:13];
    NSInteger start = self.contentOffset.x / 128;
    NSInteger end = (self.contentOffset.x + self.bounds.size.width) / 128 + 1;
    start = MAX(start, 0);
    end = MIN(end, [datasource materialsViewNumberOfMeterial:self] - 1);
    
    for (NSInteger i = start; i <= end; i++) {
        [array addObject:[NSNumber numberWithInt:i]];
    }
    
    return array;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    if (self.pool == nil) {
        self.pool = [NSMutableArray array];
        NSInteger count = [datasource materialsViewNumberOfMeterial:self];
        self.contentSize = CGSizeMake(count * 128, 128);
    }
   
    NSArray *visible = [self visibleViews];
    [visible makeObjectsPerformSelector:@selector(removeFromSuperview)];
    [self.pool addObjectsFromArray:visible];
    
    NSArray *indexes = [self visibleViewsIndexes];
    for (NSNumber *n in indexes) {
        MREMaterial *m = [datasource materialsView:self materialAtIndex:[n integerValue]];
        MREMaterialView *view = [self.pool lastObject];
        CGRect rect = CGRectMake(128 * [n intValue], 0, 128, 128);
        if (view == nil) {
            view = [[MREMaterialView alloc] initWithFrame:rect];
            [view autorelease];
        } else {
            view.frame = rect;
        }
        
        view.index = [n integerValue];
        view.imageView.image = m.image;
        view.label.text = nil;
        
        [self insertSubview:view atIndex:0];
        [self.pool removeLastObject];
    }    
}

- (void)drawRect:(CGRect)rect {
    CGContextRef currentContext = UIGraphicsGetCurrentContext();
    
    CGGradientRef glossGradient;
    CGColorSpaceRef rgbColorspace;
    size_t num_locations = 2;
    CGFloat locations[2] = { 0.0, 1.0 };
    CGFloat components[8] = { 1.0, 1.0, 1.0, 0.35,  // Start color
        1.0, 1.0, 1.0, 0.06 }; // End color
    
    rgbColorspace = CGColorSpaceCreateDeviceRGB();
    glossGradient = CGGradientCreateWithColorComponents(rgbColorspace, components, locations, num_locations);
    
    CGRect currentBounds = self.bounds;
    CGPoint topCenter = CGPointMake(CGRectGetMidX(currentBounds), 0.0f);
    CGPoint midCenter = CGPointMake(CGRectGetMidX(currentBounds), CGRectGetMidY(currentBounds));
    CGContextDrawLinearGradient(currentContext, glossGradient, topCenter, midCenter, 0);
    
    CGGradientRelease(glossGradient);
    CGColorSpaceRelease(rgbColorspace);
}

@end
