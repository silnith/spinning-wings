//
//  KSRColor.m
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 11/8/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "KSRColor.h"

@implementation KSRColor

- (id)init {
    return [self initWithRed:0
                       green:0
                        blue:0];
}

- (id)initWithRed:(GLfloat)red
            green:(GLfloat)green
             blue:(GLfloat)blue {
    self = [super init];
    
    if (self) {
        _red = red;
        _green = green;
        _blue = blue;
    }
    
    return self;
}

+ (KSRColor *) colorWithRed:(GLfloat)red
                      green:(GLfloat)green
                       blue:(GLfloat)blue {
    return [[KSRColor alloc] initWithRed:red
                                   green:green
                                    blue:blue];
}

+ (KSRColor *) black {
    return [KSRColor colorWithRed:0
                            green:0
                             blue:0];
}

+ (KSRColor *) white {
    return [KSRColor colorWithRed:1
                            green:1
                             blue:1];
}

@end
