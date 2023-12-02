//
//  WingsSaverView.h
//  WingsSaver
//
//  Created by Kent Rosenkoetter on 11/29/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import <ScreenSaver/ScreenSaver.h>

#import "KSRWingsView.h"

@interface WingsSaverView : ScreenSaverView {
    KSRWingsView *_glView;
}

+ (NSTimeInterval)frameInterval;

@end
