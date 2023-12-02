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

static GLsizei const numWings = 40;

@interface KSRWingsView : NSOpenGLView {
    GLuint wingDisplayList;
    GLuint wingDisplayLists;
    NSArray<KSRWing*> *wingList;
    
    KSRCurveGenerator *radiusCurve;
    KSRCurveGenerator *angleCurve;
    KSRCurveGenerator *deltaAngleCurve;
    KSRCurveGenerator *deltaZCurve;
    KSRCurveGenerator *rollCurve;
    KSRCurveGenerator *pitchCurve;
    KSRCurveGenerator *yawCurve;
    KSRCurveGenerator *redCurve;
    KSRCurveGenerator *greenCurve;
    KSRCurveGenerator *blueCurve;
}

- (void)advanceAnimation;

@end

NS_ASSUME_NONNULL_END
