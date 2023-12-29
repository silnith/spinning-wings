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

@interface KSRCurveGenerator : NSObject

@property (readonly) GLfloat minimumValue;
@property (readonly) GLfloat maximumValue;
@property (readonly) GLfloat valueRange;
@property (readonly) bool valueWraps;
@property (readonly) GLfloat minimumVelocity;
@property (readonly) GLfloat maximumVelocity;
@property (readonly) GLfloat minimumAcceleration;
@property (readonly) GLfloat maximumAcceleration;
@property (readonly) unsigned int ticksPerAccelerationChange;

@property (readonly) GLfloat value;
@property (readonly) GLfloat velocity;
@property (readonly) GLfloat acceleration;
@property (readonly) unsigned int ticks;

- (id)initWithValue:(GLfloat)initialValue
       minimumValue:(GLfloat)minimumValue
       maximumValue:(GLfloat)maximumValue
              wraps:(bool)wraps
    maximumVelocity:(GLfloat)maximumVelocity
maximumAcceleration:(GLfloat)maximumAcceleration
ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

- (GLfloat)getNextValue;

+ (id)curveForAngleWithValue:(GLfloat)initialValue
             maximumVelocity:(GLfloat)maximumVelocity
         maximumAcceleration:(GLfloat)maximumAcceleration
  ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

+ (id)curveForColorComponentWithValue:(GLfloat)initialValue
                      maximumVelocity:(GLfloat)maximumVelocity
                  maximumAcceleration:(GLfloat)maximumAcceleration
           ticksPerAccelerationChange:(unsigned int)ticksPerAccelerationChange;

@end

NS_ASSUME_NONNULL_END
