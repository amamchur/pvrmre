//
//  MREMaterialsView.h
//  ModelRenderEngine
//
//  Created by admin on 4/29/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MREMaterialsView;

@interface MREMaterial : NSObject

@property (nonatomic, retain) UIImage *image;
@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSString *texture;

@end

@protocol MREMaterialsViewDataSource <NSObject>

@required
- (NSInteger)materialsViewNumberOfMeterial:(MREMaterialsView *)view;
- (MREMaterial *)materialsView:(MREMaterialsView *)view materialAtIndex:(NSInteger)index;

@end

@protocol MREMaterialsViewDelegate <NSObject, UIScrollViewDelegate>

- (void)materialsView:(MREMaterialsView *)view didSelectMaterialAtIndex:(NSInteger)index;

@end

@interface MREMaterialView : UIView {
    UIImageView *imageView;
    UILabel *label;
}

@property (nonatomic, readonly) UIImageView *imageView;
@property (nonatomic, readonly) UILabel *label;

@end

@interface MREMaterialsView : UIScrollView

@property (nonatomic, assign) IBOutlet id<MREMaterialsViewDataSource> datasource;
@property (nonatomic, assign) IBOutlet id<MREMaterialsViewDelegate> delegate;

@end
