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

@interface KSRWingsView : NSOpenGLView {
    GLuint _wingDisplayList;
    GLuint _wingDisplayLists;
    NSArray<KSRWing*> *_wingList;
    
    KSRCurveGenerator *_radiusCurve;
    KSRCurveGenerator *_angleCurve;
    KSRCurveGenerator *_deltaAngleCurve;
    KSRCurveGenerator *_deltaZCurve;
    KSRCurveGenerator *_rollCurve;
    KSRCurveGenerator *_pitchCurve;
    KSRCurveGenerator *_yawCurve;
    KSRCurveGenerator *_redCurve;
    KSRCurveGenerator *_greenCurve;
    KSRCurveGenerator *_blueCurve;
}

- (void)advanceAnimation;

+ (GLsizei) numWings;

@end

NS_ASSUME_NONNULL_END
