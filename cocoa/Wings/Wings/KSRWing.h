//
//  KSRWing.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "KSRColor.h"

NS_ASSUME_NONNULL_BEGIN

@interface KSRWing : NSObject

@property (readonly) unsigned int glDisplayList;
@property (readonly) float radius;
@property (readonly) float angle;
@property (readonly) float deltaAngle;
@property (readonly) float deltaZ;
@property (readonly) float roll;
@property (readonly) float pitch;
@property (readonly) float yaw;
@property (readonly) KSRColor * color;
@property (readonly) KSRColor * edgeColor;

- (id)initWithGLDisplayList:(unsigned int) glDisplayList;

- (id)initWithGLDisplayList:(unsigned int) glDisplayList
                     radius:(float) radius
                      angle:(float) angle
                 deltaAngle:(float) deltaAngle
                     deltaZ:(float) deltaZ
                       roll:(float) roll
                      pitch:(float) pitch
                        yaw:(float) yaw
                      color:(KSRColor *) color
                  edgeColor:(KSRColor *) edgeColor;

@end

NS_ASSUME_NONNULL_END
