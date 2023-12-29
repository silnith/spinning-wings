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

@interface KSRColor : NSObject

@property (readonly) GLfloat red;
@property (readonly) GLfloat green;
@property (readonly) GLfloat blue;

- (id)initWithRed:(GLfloat)red
            green:(GLfloat)green
             blue:(GLfloat)blue;

+ (KSRColor *) colorWithRed:(GLfloat)red
                      green:(GLfloat)green
                       blue:(GLfloat)blue;

+ (KSRColor *) black;

+ (KSRColor *) white;

@end

NS_ASSUME_NONNULL_END
