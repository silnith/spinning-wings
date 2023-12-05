//
//  KSRWingsView.h
//  WingsSaver
//
//  Created by Kent Rosenkoetter on 11/30/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <OpenGL/gl.h>

#import "Wings/KSRWing.h"
#import "Wings/KSRCurveGenerator.h"

NS_ASSUME_NONNULL_BEGIN

@interface KSRWingsView : NSOpenGLView

@property GLuint glMajorVersion;
@property GLuint glMinorVersion;

@property GLuint wingDisplayList;

@property NSArray<KSRWing *> * wingList;

@property KSRCurveGenerator * radiusCurve;
@property KSRCurveGenerator * angleCurve;
@property KSRCurveGenerator * deltaAngleCurve;
@property KSRCurveGenerator * deltaZCurve;
@property KSRCurveGenerator * rollCurve;
@property KSRCurveGenerator * pitchCurve;
@property KSRCurveGenerator * yawCurve;
@property KSRCurveGenerator * redCurve;
@property KSRCurveGenerator * greenCurve;
@property KSRCurveGenerator * blueCurve;

- (void)advanceAnimation;

+ (GLsizei)numWings;

@end

NS_ASSUME_NONNULL_END
