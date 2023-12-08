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

@interface WingsSaverView ()

@property (readonly) KSRWingsView * glView;

+ (NSTimeInterval)frameInterval;

@end

@implementation WingsSaverView

// The length of time, in seconds, to display each frame of animation.
+ (NSTimeInterval)frameInterval
{
    return 1.0 / 30.0;
}

- (instancetype)init
{
    self = [super init];
    
    NSLog(@"%@:init", self);
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
            NSOpenGLPFAAllowOfflineRenderers,
            0,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:NSZeroRect pixelFormat:format];
        
        if (_glView) {
//            NSLog(@"%@:Found OpenGL pixel format:%@", self, format);
        } else {
            NSLog(@"%@:No OpenGL view.", self);
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
    }
    
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
    self = [super initWithCoder:decoder];
    
    NSLog(@"%@:initWithCoder:%@", self, decoder);
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
            NSOpenGLPFAAllowOfflineRenderers,
            0,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:NSZeroRect pixelFormat:format];
        
        if (_glView) {
//            NSLog(@"%@:Found OpenGL pixel format:%@", self, format);
        } else {
            NSLog(@"%@:No OpenGL view.", self);
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
    }
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    
    NSLog(@"%@:initWithFrame:{%f, %f, %f, %f}",
          self, frameRect.origin.x, frameRect.origin.y, frameRect.size.width, frameRect.size.height);
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
            NSOpenGLPFAAllowOfflineRenderers,
            0,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:frameRect pixelFormat:format];
        
        if (_glView) {
//            NSLog(@"%@:Found OpenGL pixel format:%@", self, format);
        } else {
            NSLog(@"%@:No OpenGL view.", self);
            
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
    
    NSLog(@"%@:initWithFrame:{%f, %f, %f, %f}, isPreview:%d",
          self, frame.origin.x, frame.origin.y, frame.size.width, frame.size.height, isPreview);
    
    if (self) {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 32,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAMaximumPolicy,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
            NSOpenGLPFAAllowOfflineRenderers,
            0,
        };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        _glView = [[KSRWingsView alloc] initWithFrame:frame pixelFormat:format];
        
        if (_glView) {
//            NSLog(@"%@:Found OpenGL pixel format:%@", self, format);
        } else {
            NSLog(@"%@:No OpenGL view.", self);
            
            return nil;
        }
        
        [_glView setWantsBestResolutionOpenGLSurface:YES];
        [_glView setWantsExtendedDynamicRangeOpenGLSurface:YES];
        
        [self addSubview:_glView];
        
        [self setAnimationTimeInterval:[WingsSaverView frameInterval]];
    }
    
    return self;
}

- (void)animateOneFrame
{
    [self.glView advanceAnimation];
}

- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
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
