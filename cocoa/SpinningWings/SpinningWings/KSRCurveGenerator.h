//
//  KSRCurveGenerator.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <OpenGL/gl.h>

NS_ASSUME_NONNULL_BEGIN

/**
 A class that produces a sequence of numbers that slowly shift within predefined boundaries.
 */
@interface KSRCurveGenerator : NSObject

/**
 The minimum allowed value for the curve.
 */
@property (readonly) GLfloat minimumValue;
/**
 The maximum allowed value for the curve.
 */
@property (readonly) GLfloat maximumValue;
@property (readonly) GLfloat valueRange;
/**
 Whether curve values wrap between maximum and minimum when they overflow.
 */
@property (readonly) bool valueWraps;
@property (readonly) GLfloat minimumVelocity;
/**
 The maximum rate at which the curve value is allowed to change.
 */
@property (readonly) GLfloat maximumVelocity;
@property (readonly) GLfloat minimumAcceleration;
/**
 The maximum acceleration for curve changes.
 */
@property (readonly) GLfloat maximumAcceleration;
/**
 The number of curve values to return before changing the curve acceleration.
 */
@property (readonly) unsigned int ticksPerAccelerationChange;

/**
 The current value of the curve.
 */
@property (readonly) GLfloat value;
/**
 The current velocity of the curve.  This is the amount the curve value has changed since the last value.
 */
@property (readonly) GLfloat velocity;
/**
 The current acceleration of the curve.  This is the amount that the curve velocity has changed since the last curve value.
 */
@property (readonly) GLfloat acceleration;
/**
 The number of curve values retrieved since the last time the curve acceleration changed.
 */
@property (readonly) unsigned int ticks;

/**
 Initializes a new curve generator.

 @param initialValue The initial value of the curve.
 @param minimumValue The minimum value of the curve.
 @param maximumValue The maximum value of the curve.
 @param wraps Whether curve values map from the maximum value to the minimum value.
 @param maximumVelocity The maximum velocity, or slope of the curve.
 @param maximumAcceleration The maximum acceleration for curve changes.
 @param ticksPerAccelerationChange The number of values to generate before the acceleration changes.
 @return The initialized curve generator.
 */
- (id)initWithValue:(GLfloat)initialValue
       minimumValue:(GLfloat)minimumValue
       maximumValue:(GLfloat)maximumValue
              wraps:(bool)wraps
    maximumVelocity:(GLfloat)maximumVelocity
maximumAcceleration:(GLfloat)maximumAcceleration
ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

/**
 Advances the tick count, applies velocity and acceleration, and returns the next value for the random curve.

 @return The next value of the curve.
 */
- (GLfloat)getNextValue;

/**
 Returns a KSRCurveGenerator for angles.
 The values will be in the range [0, 360), and will wrap.

 @param initialValue The initial angle.
 @param maximumVelocity The maximum angle change per tick.
 @param maximumAcceleration The maximum acceleration.
 @param ticksPerAccelerationChange The number of values to generate before the acceleration changes.
 @return A KSRCurveGenerator that returns values that can be used as angles.
 */
+ (id)curveForAngleWithValue:(GLfloat)initialValue
             maximumVelocity:(GLfloat)maximumVelocity
         maximumAcceleration:(GLfloat)maximumAcceleration
  ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

/**
 Returns a KSRCurveGenerator for color component values.
 In other words, red, green, or blue.
 The values will be in the range [0, 1], and will clamp without wrapping.

 @param initialValue The initial color (component) value.
 @param maximumVelocity The maximum color (component) change per tick.
 @param maximumAcceleration The maximum acceleration.
 @param ticksPerAccelerationChange The number of values to generate before the acceleration changes.
 @return A KSRCurveGenerator that returns values specific for color components.
 */
+ (id)curveForColorComponentWithValue:(GLfloat)initialValue
                      maximumVelocity:(GLfloat)maximumVelocity
                  maximumAcceleration:(GLfloat)maximumAcceleration
           ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

@end

NS_ASSUME_NONNULL_END
