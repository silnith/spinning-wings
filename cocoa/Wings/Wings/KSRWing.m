//
//  KSRWing.m
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "KSRWing.h"

@implementation KSRWing

- (id)init {
    return [self initWithGLDisplayList:0];
}

- (id)initWithGLDisplayList:(unsigned int)glDisplayList {
    return [self initWithGLDisplayList:glDisplayList
                                radius:10 angle:0
                            deltaAngle:15 deltaZ:0.5
                                  roll:0 pitch:0 yaw:0
                                 color:[KSRColor black] edgeColor:[KSRColor white]];
}

- (id)initWithGLDisplayList:(unsigned int)glDisplayList
                     radius:(float)radius angle:(float)angle
                 deltaAngle:(float)deltaAngle deltaZ:(float)deltaZ
                       roll:(float)roll pitch:(float)pitch yaw:(float)yaw
                      color:(KSRColor *)color edgeColor:(KSRColor *)edgeColor {
    self = [super init];
    
    if (self) {
        _glDisplayList = glDisplayList;
        _radius = radius;
        _angle = angle;
        _deltaAngle = deltaAngle;
        _deltaZ = deltaZ;
        _roll = roll;
        _pitch = pitch;
        _yaw = yaw;
        _color = color;
        _edgeColor = edgeColor;
    }
    
    return self;
}

@end
