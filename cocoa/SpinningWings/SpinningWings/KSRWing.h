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

/**
 A single quad spinning around the central axis.
 
 The graphics hack is componsed of several wings rendered in relation to a central axis.
 The axis functions like a pole sticking straight up.
 
 Conceptually there is only one wing that slowly changes over time.
 However, several copies of the wing are drawn, each copy being
 the state of the wing in the previous render tick.  Therefore
 if there are ten wings drawn, the top is exactly the wing that
 was at the bottom ten ticks before, and the bottom is the most
 recent wing generated.
 
 All angles are in degrees.
 */
@interface KSRWing : NSObject

/**
 The identifier for the OpenGL display list that renders this wing.
 */
@property (readonly) GLuint glDisplayList;
/**
 The distance of the wing from the central axis around which they all rotate.
 */
@property (readonly) GLfloat radius;
/**
 The angle of the wing around the central axis.
 
 All angles are in degrees.
 */
@property (readonly) GLfloat angle;
/**
 The additional angle around the central axis that each subsequent wing should be rendered.
 
 This is the angle between this wing and the previous wing.
 */
@property (readonly) GLfloat deltaAngle;
/**
 The additional distance up the central axis that each subsequent wing should be rendered.
 */
@property (readonly) GLfloat deltaZ;
/**
 The roll of the wing.  This is specific to the wing itself.
 */
@property (readonly) GLfloat roll;
/**
 The pitch of the wing.
 */
@property (readonly) GLfloat pitch;
/**
 The yaw of the wing.
 */
@property (readonly) GLfloat yaw;
/**
 The color of the wing.
 */
@property (readonly) KSRColor * color;
/**
 The color of the wing edge.
 */
@property (readonly) KSRColor * edgeColor;

- (id)initWithGLDisplayList:(GLuint) glDisplayList;

/**
 Initializes a new wing with the provided parameters.

 @param glDisplayList The OpenGL display list identifier used for the rendering list for this wing.
 @param radius The radius from the central axis.
 @param angle The angle around the central axis.
 @param deltaAngle The additional angle around the central axis as the wing recedes into history.
 @param deltaZ The additional height along the central axis as the wing recedes into history.
 @param roll The roll of the wing.
 @param pitch The pitch of the wing.
 @param yaw The yaw of the wing.
 @param color The color of the wing.
 @param edgeColor The color of the edge of the wing.
 @return The initialized wing object.
 */
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
