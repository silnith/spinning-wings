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
@property (weak) IBOutlet KSRWingsView *wingsView;

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
    NSTimeInterval interval = [AppDelegate frameInterval];
    NSMethodSignature * methodSignature = [KSRWingsView instanceMethodSignatureForSelector:@selector(advanceAnimation)];
    NSInvocation * invocation = [NSInvocation invocationWithMethodSignature:methodSignature];
    invocation.selector = @selector(advanceAnimation);
    invocation.target = self.wingsView;
    self.timer = [NSTimer scheduledTimerWithTimeInterval:interval invocation:invocation repeats:YES];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    [self.timer invalidate];
    self.timer = nil;
}


@end
