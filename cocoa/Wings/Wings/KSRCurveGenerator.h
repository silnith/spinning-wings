//
//  KSRCurveGenerator.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KSRCurveGenerator : NSObject

@property (readonly) float minimumValue;
@property (readonly) float maximumValue;
@property (readonly) bool valueWraps;
@property (readonly) float maximumVelocity;
@property (readonly) float maximumAcceleration;
@property (readonly) unsigned int ticksPerAccelerationChange;
@property (nonatomic) float value;
@property (nonatomic) float velocity;
@property float acceleration;
@property unsigned int ticks;

- (id)initWithValue:(float)initialValue
       minimumValue:(float)minimumValue
       maximumValue:(float)maximumValue
              wraps:(bool)wraps
    maximumVelocity:(float)maximumVelocity
maximumAcceleration:(float)maximumAcceleration
ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

+ (id)curveForAngleWithValue:(float)initialValue
                              maximumVelocity:(float)maximumVelocity
                          maximumAcceleration:(float)maximumAcceleration
                   ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

+ (id)curveForColorComponentWithValue:(float)initialValue
                                       maximumVelocity:(float)maximumVelocity
                                   maximumAcceleration:(float)maximumAcceleration
                            ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

- (float)getNextValue;

@end

NS_ASSUME_NONNULL_END
