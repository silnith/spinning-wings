//
//  WingsSaverView.m
//  WingsSaver
//
//  Created by Kent Rosenkoetter on 11/29/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "WingsSaverView.h"

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

@implementation WingsSaverView

// The length of time, in seconds, to display each frame of animation.
+ (NSTimeInterval)frameInterval {
    return 1.0 / 30.0;
}

- (instancetype)init {
    self = [super init];
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, kCGLOGLPVersion_Legacy,
            NSOpenGLPFAAllowOfflineRenderers,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:NSZeroRect pixelFormat:format];
        
        if (_glView) {
            NSLog(@"Found OpenGL pixel format:%@", format);
        } else {
            NSLog(@"No OpenGL view.");
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
    }
    
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    self = [super initWithCoder:decoder];
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, kCGLOGLPVersion_Legacy,
            NSOpenGLPFAAllowOfflineRenderers,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:NSZeroRect pixelFormat:format];
        
        if (_glView) {
            NSLog(@"Found OpenGL pixel format:%@", format);
        } else {
            NSLog(@"No OpenGL view.");
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
    }
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    
    NSLog(@"initWithFrame:{%f, %f, %f, %f}",
          frameRect.origin.x, frameRect.origin.y, frameRect.size.width, frameRect.size.height);
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, kCGLOGLPVersion_Legacy,
            NSOpenGLPFAAllowOfflineRenderers,
            (NSOpenGLPixelFormatAttribute)nil,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:frameRect pixelFormat:format];
        
        if (_glView) {
            NSLog(@"Found OpenGL pixel format:%@", format);
        } else {
            NSLog(@"No OpenGL view.");
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
    }
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    
    NSLog(@"initWithFrame:{%f, %f, %f, %f}, isPreview:%d",
          frame.origin.x, frame.origin.y, frame.size.width, frame.size.height, isPreview);
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, kCGLOGLPVersion_Legacy,
            NSOpenGLPFAAllowOfflineRenderers,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:frame pixelFormat:format];
        
        if (_glView) {
            NSLog(@"Found OpenGL pixel format:%@", format);
        } else {
            NSLog(@"No OpenGL view.");
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
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
    
    [_glView advanceAnimation];
    
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
