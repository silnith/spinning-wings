//
//  KSRGLInfo.h
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 3/12/26.
//  Copyright © 2026 Silnith.org. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <OpenGL/gl.h>

NS_ASSUME_NONNULL_BEGIN

/**
 A simple class to query and expose basic information about the OpenGL
 implementation.  This class makes OpenGL queries, so the GL context
 must already exist.
 
 Starting with OpenGL 3.0, this can be replaced with calls that query
 the version number directly.
 
 GLint glMajorVersion = 1;
 GLint glMinorVersion = 0;
 glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
 glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
 
 assert(glMajorVersion == 3);
 assert(glMinorVersion == 0);
 */
@interface KSRGLInfo : NSObject

/**
 The major version of the OpenGL implementation.
 */
@property (readonly) GLuint majorVersion;

/**
 The minor version of the OpenGL implementation.
 */
@property (readonly) GLuint minorVersion;

/**
 Queries the OpenGL context for version information.

 @return an initialized KSRGLInfo object
 */
- (id)init;

/**
 Parses the given OpenGL version string.
 This should come from a call to glGetString(GL_VERSION).

 @param cString A version string.
 @return an initialized KSRGLInfo object
 */
- (id)initWithCString:(char const *)cString;

/**
 Parses the given OpenGL version string.
 This should come from a call to glGetString(GL_VERSION).

 @param versionString A version string.
 @return an initialized KSRGLInfo object
 */
- (id)initWithVersionString:(NSString *)versionString;

/**
 Returns YES if the OpenGL implementation is equal to or higher
 than the input version number.

 @param major The major version number expected.
 @param minor The minor version number expected.
 @return YES if the OpenGL implementation satisfies the requested version.
 */
- (bool)isAtLeastVersionMajor:(GLuint)major
                        Minor:(GLuint)minor;

@end

NS_ASSUME_NONNULL_END
