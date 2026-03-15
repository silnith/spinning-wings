//
//  KSRGLInfo.m
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 3/12/26.
//  Copyright © 2026 Silnith.org. All rights reserved.
//

#import "KSRGLInfo.h"

@interface KSRGLInfo()

@property (readwrite) GLuint majorVersion;
@property (readwrite) GLuint minorVersion;

@end

@implementation KSRGLInfo

- (id)init {
    GLubyte const * const glVersion = glGetString(GL_VERSION);
    return [self initWithCString:(char const *)glVersion];
}

- (id)initWithCString:(char const *)cString {
    return [self initWithVersionString:@(cString)];
}

- (id)initWithVersionString:(NSString *)versionString {
    self = [super init];
    
    if (self) {
        NSScanner * scanner = [NSScanner scannerWithString:versionString];
        scanner.charactersToBeSkipped = [NSCharacterSet characterSetWithCharactersInString:@""];
        unsigned long long major;
        if ([scanner scanUnsignedLongLong:&major]) {
            _majorVersion = [@(major) unsignedIntValue];
            
            scanner.charactersToBeSkipped = [NSCharacterSet characterSetWithCharactersInString:@"."];
            unsigned long long minor;
            if ([scanner scanUnsignedLongLong:&minor]) {
                _minorVersion = [@(minor) unsignedIntValue];
            }
        }
    }
    
    return self;
}

- (bool)isAtLeastVersionMajor:(GLuint)major
                        Minor:(GLuint)minor {
    return (self.majorVersion > major) || (self.majorVersion == major && self.minorVersion >= minor);
}

@end
