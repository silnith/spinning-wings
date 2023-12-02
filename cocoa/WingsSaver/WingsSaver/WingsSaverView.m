//
//  WingsSaverView.m
//  WingsSaver
//
//  Created by Kent Rosenkoetter on 11/29/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "WingsSaverView.h"

#import "KSRWingsView.h"

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#import <Foundation/NSDebug.h>
#import <syslog.h>

@implementation WingsSaverView

- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    
    NSLog(@"initWithFrame:{%f, %f, %f, %f}",
          frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
//    NSLog(@"initWithFrame:%@", frame);
    
//    NSDebugEnabled = true;
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
//            NSOpenGLPFAOpenGLProfile,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        glView = [[KSRWingsView alloc] initWithFrame:frame pixelFormat:format];
        
        if (!glView) {
            NSLog(@"No OpenGL view.");
            
            return nil;
        } else {
            NSLog(@"Found OpenGL pixel format.");
        }
        
        [glView setWantsBestResolutionOpenGLSurface:YES];
        
        [self addSubview:glView];
        
        [self setAnimationTimeInterval:1/30.0];
//        [self setAnimationTimeInterval:1];
    }
    
    return self;
}

- (void)startAnimation
{
    [super startAnimation];
    
    NSLog(@"startAnimation");
}

- (void)stopAnimation
{
    [super stopAnimation];
    
    NSLog(@"stopAnimation");
}

- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
    
    NSLog(@"drawRect:{%f, %f, %f, %f}",
          rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

- (void)animateOneFrame
{
    NSLog(@"Animating one frame.");
    
    [glView advanceAnimation];
    
    [self setNeedsDisplay:YES];
    
    NSLog(@"animateOneFrame");
}

- (BOOL)hasConfigureSheet
{
    return NO;
}

- (NSWindow*)configureSheet
{
    return nil;
}

@end
