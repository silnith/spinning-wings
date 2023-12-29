//
//  KSRWing.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <OpenGL/gl.h>

#import "KSRColor.h"

NS_ASSUME_NONNULL_BEGIN

@interface KSRWing : NSObject

@property (readonly) GLuint glDisplayList;
@property (readonly) GLfloat radius;
@property (readonly) GLfloat angle;
@property (readonly) GLfloat deltaAngle;
@property (readonly) GLfloat deltaZ;
@property (readonly) GLfloat roll;
@property (readonly) GLfloat pitch;
@property (readonly) GLfloat yaw;
@property (readonly) KSRColor * color;
@property (readonly) KSRColor * edgeColor;

- (id)initWithGLDisplayList:(GLuint) glDisplayList;

- (id)initWithGLDisplayList:(GLuint) glDisplayList
                     radius:(GLfloat) radius
                      angle:(GLfloat) angle
                 deltaAngle:(GLfloat) deltaAngle
                     deltaZ:(GLfloat) deltaZ
                       roll:(GLfloat) roll
                      pitch:(GLfloat) pitch
                        yaw:(GLfloat) yaw
                      color:(KSRColor *) color
                  edgeColor:(KSRColor *) edgeColor;

@end

NS_ASSUME_NONNULL_END
