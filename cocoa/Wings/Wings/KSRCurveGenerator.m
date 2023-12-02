//
//  KSRCurveGenerator.m
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "KSRCurveGenerator.h"

#import <ScreenSaver/ScreenSaver.h>

@implementation KSRCurveGenerator

- (id)init {
    return [self initWithValue:0
                  minimumValue:0 maximumValue:0 wraps:false
               maximumVelocity:0 maximumAcceleration:0 ticksPerAccelerationChange:0];
}

- (id)initWithValue:(float)initialValue
       minimumValue:(float)minimumValue
       maximumValue:(float)maximumValue
              wraps:(bool)wraps
    maximumVelocity:(float)maximumVelocity
maximumAcceleration:(float)maximumAcceleration
ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange {
    self = [super init];
    
    if (self) {
        _minimumValue = minimumValue;
        _maximumValue = maximumValue;
        _valueWraps = wraps;
        _maximumVelocity = maximumVelocity;
        _maximumAcceleration = maximumAcceleration;
        _ticksPerAccelerationChange = ticksPerAccelerationChange;
        _value = initialValue;
        _velocity = 0;
        _acceleration = 0;
        _ticks = ticksPerAccelerationChange;
    }
    
    return self;
}

+ (id)curveForAngleWithValue:(float)initialValue
             maximumVelocity:(float)maximumVelocity
         maximumAcceleration:(float)maximumAcceleration
  ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange {
    return [[KSRCurveGenerator alloc] initWithValue:initialValue
                                       minimumValue:0
                                       maximumValue:360
                                              wraps:true
                                    maximumVelocity:maximumVelocity
                                maximumAcceleration:maximumAcceleration
                         ticksPerAccelerationChange:ticksPerAccelerationChange];
}

+ (id)curveForColorComponentWithValue:(float)initialValue
                      maximumVelocity:(float)maximumVelocity
                  maximumAcceleration:(float)maximumAcceleration
           ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange {
    return [[KSRCurveGenerator alloc] initWithValue:initialValue
                                       minimumValue:0
                                       maximumValue:1
                                              wraps:false
                                    maximumVelocity:maximumVelocity
                                maximumAcceleration:maximumAcceleration
                         ticksPerAccelerationChange:ticksPerAccelerationChange];
}

+ (float)clamp:(float)value
  minimumValue:(float)minimumValue
  maximumValue:(float)maximumValue {
    return fminf(fmaxf(value, minimumValue), maximumValue);
}

- (void)setValue:(float)value {
    if (self.valueWraps) {
        _value = fmodf(value - self.minimumValue,
                       self.maximumValue - self.minimumValue) + self.minimumValue;
    } else {
        _value = [KSRCurveGenerator clamp:value
                             minimumValue:self.minimumValue
                             maximumValue:self.maximumValue];
    }
}

- (void)setVelocity:(float)velocity {
    _velocity = [KSRCurveGenerator clamp:velocity
                            minimumValue:-self.maximumVelocity
                            maximumValue:self.maximumVelocity];
}

- (void)advanceTick {
    if (++self.ticks > self.ticksPerAccelerationChange) {
        self.acceleration = SSRandomFloatBetween(-self.maximumAcceleration, self.maximumAcceleration);
        self.ticks = 0;
    }
    self.velocity += self.acceleration;
    self.value += self.velocity;
}

- (float)getNextValue {
    [self advanceTick];
    return self.value;
}

@end
