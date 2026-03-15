//
//  KSRGLInfoTest.m
//  SpinningWingsTests
//
//  Created by Kent Rosenkoetter on 3/12/26.
//  Copyright © 2026 Silnith.org. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "KSRGLInfo.h"

@interface KSRGLInfoTest : XCTestCase

@end

@implementation KSRGLInfoTest

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testParseCString {
    char const * cString = "2.3.1";
    
    KSRGLInfo * info = [[KSRGLInfo alloc] initWithCString:cString];
    
    XCTAssertEqual(2, info.majorVersion);
    XCTAssertEqual(3, info.minorVersion);
}

- (void)testParseVersionString {
    NSString * versionString = @"2.3.1";
    
    KSRGLInfo * info = [[KSRGLInfo alloc] initWithVersionString:versionString];
    
    XCTAssertEqual(2, info.majorVersion);
    XCTAssertEqual(3, info.minorVersion);
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
