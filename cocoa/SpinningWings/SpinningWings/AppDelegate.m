//
//  AppDelegate.m
//  SpinningWings
//
//  Created by Kent Rosenkoetter on 12/29/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "AppDelegate.h"

#import "KSRWingsView.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;

@property NSTimer * timer;

+ (NSTimeInterval)frameInterval;

@end

@implementation AppDelegate

// The length of time, in seconds, to display each frame of animation.
+ (NSTimeInterval)frameInterval
{
    return 1.0 / 30.0;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // TODO: Replace this with idiomatic IBOutlet stuff.
    KSRWingsView * wingsView = nil;
    
    for (NSView * view in self.window.contentView.subviews) {
        if ([view isKindOfClass:[KSRWingsView class]]) {
            wingsView = (KSRWingsView *) view;
        }
    }
    
    NSTimeInterval interval = [AppDelegate frameInterval];
    NSMethodSignature * methodSignature = [KSRWingsView instanceMethodSignatureForSelector:@selector(advanceAnimation)];
    NSInvocation * invocation = [NSInvocation invocationWithMethodSignature:methodSignature];
    invocation.selector = @selector(advanceAnimation);
    invocation.target = wingsView;
    self.timer = [NSTimer scheduledTimerWithTimeInterval:interval invocation:invocation repeats:YES];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    [self.timer invalidate];
    self.timer = nil;
}


@end
