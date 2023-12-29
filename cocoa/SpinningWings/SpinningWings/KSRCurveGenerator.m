//
//  KSRCurveGenerator.m
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "KSRCurveGenerator.h"

#import <ScreenSaver/ScreenSaver.h>

@interface KSRCurveGenerator ()

@property (readwrite) GLfloat value;
@property (readwrite) GLfloat velocity;
@property (readwrite) GLfloat acceleration;
@property (readwrite) unsigned int ticks;

+ (GLfloat)clamp:(GLfloat)value
    minimumValue:(GLfloat)minimumValue
    maximumValue:(GLfloat)maximumValue;

@end

@implementation KSRCurveGenerator

- (id)init {
    return [self initWithValue:0.0f
                  minimumValue:0.0f maximumValue:0.0f wraps:false
               maximumVelocity:0.0f maximumAcceleration:0.0f ticksPerAccelerationChange:0];
}

- (id)initWithValue:(GLfloat)initialValue
       minimumValue:(GLfloat)minimumValue
       maximumValue:(GLfloat)maximumValue
              wraps:(bool)wraps
    maximumVelocity:(GLfloat)maximumVelocity
maximumAcceleration:(GLfloat)maximumAcceleration
ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange {
    self = [super init];
    
    if (self) {
        _minimumValue = minimumValue;
        _maximumValue = maximumValue;
        _valueRange = maximumValue - minimumValue;
        _valueWraps = wraps;
        _minimumVelocity = -maximumVelocity;
        _maximumVelocity = maximumVelocity;
        _minimumAcceleration = -maximumAcceleration;
        _maximumAcceleration = maximumAcceleration;
        _ticksPerAccelerationChange = ticksPerAccelerationChange;
        
        _value = initialValue;
        _velocity = 0;
        _acceleration = 0;
        /*
         * The current tick is initialized to the value that triggers an
         * acceleration change so that the very first advance triggers a change
         * in the acceleration.
         */
        _ticks = ticksPerAccelerationChange;
    }
    
    return self;
}

@synthesize value = _value;

- (GLfloat)value {
    return _value;
}

- (void)setValue:(GLfloat)value {
    if (self.valueWraps) {
        value = fmodf(value - self.minimumValue, self.valueRange) + self.minimumValue;
    } else {
        value = [KSRCurveGenerator clamp:value
                            minimumValue:self.minimumValue
                            maximumValue:self.maximumValue];
    }
    _value = value;
}

@synthesize velocity = _velocity;

- (GLfloat)velocity {
    return _velocity;
}

- (void)setVelocity:(GLfloat)velocity {
    _velocity = [KSRCurveGenerator clamp:velocity
                            minimumValue:self.minimumVelocity
                            maximumValue:self.maximumVelocity];
}

- (void)advanceTick {
    if (++self.ticks > self.ticksPerAccelerationChange) {
        self.acceleration = SSRandomFloatBetween(_minimumAcceleration, self.maximumAcceleration);
        self.ticks = 0;
    }
    self.velocity += self.acceleration;
    self.value += self.velocity;
}

- (GLfloat)getNextValue {
    [self advanceTick];
    return self.value;
}

+ (GLfloat)clamp:(GLfloat)value
    minimumValue:(GLfloat)minimumValue
    maximumValue:(GLfloat)maximumValue {
    return fminf(fmaxf(value, minimumValue), maximumValue);
}

+ (id)curveForAngleWithValue:(GLfloat)initialValue
             maximumVelocity:(GLfloat)maximumVelocity
         maximumAcceleration:(GLfloat)maximumAcceleration
  ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange {
    return [[KSRCurveGenerator alloc] initWithValue:initialValue
                                       minimumValue:0
                                       maximumValue:360
                                              wraps:true
                                    maximumVelocity:maximumVelocity
                                maximumAcceleration:maximumAcceleration
                         ticksPerAccelerationChange:ticksPerAccelerationChange];
}

+ (id)curveForColorComponentWithValue:(GLfloat)initialValue
                      maximumVelocity:(GLfloat)maximumVelocity
                  maximumAcceleration:(GLfloat)maximumAcceleration
           ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange {
    return [[KSRCurveGenerator alloc] initWithValue:initialValue
                                       minimumValue:0
                                       maximumValue:1
                                              wraps:false
                                    maximumVelocity:maximumVelocity
                                maximumAcceleration:maximumAcceleration
                         ticksPerAccelerationChange:ticksPerAccelerationChange];
}

@end
