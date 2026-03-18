//
//  KSRColor.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <OpenGL/gl.h>

NS_ASSUME_NONNULL_BEGIN

/**
 A simple RGB color value.
 */
@interface KSRColor : NSObject

/**
 The red component.
 
 This should be in the range [0, 1].
 */
@property (readonly) GLfloat red;

/**
 The green component.
 
 This should be in the range [0, 1].
 */
@property (readonly) GLfloat green;

/**
 The blue component.
 
 This should be in the range [0, 1].
 */
@property (readonly) GLfloat blue;

/**
 Initializes a new color object.
 
 Values for color components should be in the range [0.0, 1.0].

 @param red The red component, in the range [0, 1].
 @param green The green component, in the range [0, 1].
 @param blue The blue component, in the range [0, 1].
 @return The initialized color object.
 */
- (id)initWithRed:(GLfloat)red
            green:(GLfloat)green
             blue:(GLfloat)blue;

/**
 A factory for color objects.

 @param red The red component, in the range [0, 1].
 @param green The green component, in the range [0, 1].
 @param blue The blue component, in the range [0, 1].
 @return An initialized color object.
 */
+ (KSRColor *) colorWithRed:(GLfloat)red
                      green:(GLfloat)green
                       blue:(GLfloat)blue;

/**
 A constant for the color black.
 The color components are all 0.

 @return The color black.
 */
+ (KSRColor *) black;

/**
 A constant for the color white.
 The color components are all 1.

 @return The color white.
 */
+ (KSRColor *) white;

@end

NS_ASSUME_NONNULL_END
