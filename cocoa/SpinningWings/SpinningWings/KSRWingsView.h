//
//  KSRWingsView.h
//  WingsSaver
//
//  Created by Kent Rosenkoetter on 11/30/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <OpenGL/gl.h>

#import "KSRCurveGenerator.h"
#import "KSRWing.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A view that encapsulates all of the logic and data for initializing,
 animating, and rendering the 3D view of the spinning wings.
 */
@interface KSRWingsView : NSOpenGLView

@property GLsizei numWings;

@property (readonly) NSArray<KSRWing *> * wingList;

/**
 Advances the spinning wings animation by one frame.
 */
- (void)advanceAnimation;

@end

NS_ASSUME_NONNULL_END
