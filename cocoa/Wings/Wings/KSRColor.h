//
//  KSRColor.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KSRColor : NSObject

@property (readonly) float red;
@property (readonly) float green;
@property (readonly) float blue;

- (id)initWithRed:(float)red green:(float)green blue:(float)blue;

+ (KSRColor *) black;
+ (KSRColor *) white;

@end

NS_ASSUME_NONNULL_END
