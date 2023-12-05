//
//  KSRWingsView.m
//  WingsSaver
//
//  Created by Kent Rosenkoetter on 11/30/23.
//  Copyright © 2023 Silnith.org. All rights reserved.
//

#import "KSRWingsView.h"

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

@implementation KSRWingsView

+ (void)describePixelFormat:(NSOpenGLPixelFormat *)format {
    for (GLint virtualScreen = 0; virtualScreen < format.numberOfVirtualScreens; virtualScreen++) {
        NSLog(@"KSRWingsView:%@:Virtual Screen: %d", format, virtualScreen);
        
        GLint glProfile;
        [format getValues:&glProfile forAttribute:NSOpenGLPFAOpenGLProfile forVirtualScreen:virtualScreen];
        NSString * glProfileString;
        switch (glProfile) {
            case NSOpenGLProfileVersionLegacy:
                glProfileString = @"Legacy";
                break;
                
            case NSOpenGLProfileVersion3_2Core:
                glProfileString = @"3.2 Core";
                break;
                
            case NSOpenGLProfileVersion4_1Core:
                glProfileString = @"4.1 Core";
                break;
                
            default:
                glProfileString = [[NSNumber numberWithLong:glProfile] stringValue];
                break;
        }
        NSLog(@"KSRWingsView:%@:OpenGL Profile: %@", format, glProfileString);
        
        GLint compliant;
        [format getValues:&compliant forAttribute:NSOpenGLPFACompliant forVirtualScreen:virtualScreen];
        NSLog(@"KSRWingsView:%@:Compliant: %d", format, compliant);
        
        GLint accelerated;
        [format getValues:&accelerated forAttribute:NSOpenGLPFAAccelerated forVirtualScreen:virtualScreen];
        NSLog(@"KSRWingsView:%@:Accelerated: %d", format, accelerated);
        
        GLint doubleBuffer;
        [format getValues:&doubleBuffer forAttribute:NSOpenGLPFADoubleBuffer forVirtualScreen:virtualScreen];
        NSLog(@"KSRWingsView:%@:Double Buffer: %d", format, doubleBuffer);
        
        GLint colorSize;
        [format getValues:&colorSize forAttribute:NSOpenGLPFAColorSize forVirtualScreen:virtualScreen];
        NSLog(@"KSRWingsView:%@:Color Size: %d", format, colorSize);
        
        GLint depthSize;
        [format getValues:&depthSize forAttribute:NSOpenGLPFADepthSize forVirtualScreen:virtualScreen];
        NSLog(@"KSRWingsView:%@:Depth Size: %d", format, depthSize);
    }
}

+ (GLsizei)numWings {
    return 40;
}

- (instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
    self = [super initWithFrame:frameRect pixelFormat:format];
    
    NSLog(@"%@:initWithFrame:{%f, %f, %f, %f} pixelFormat:%@",
          self,
          frameRect.origin.x, frameRect.origin.y, frameRect.size.width, frameRect.size.height,
          format);
    
//    [KSRWingsView describePixelFormat:format];
    
    if (self) {
        [self setFrameOrigin:NSZeroPoint];
        
        GLint swapInterval = 1;
        [self.openGLContext setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];
        
        _glMajorVersion = 1;
        _glMinorVersion = 0;
        
        _wingDisplayList = 0;
        
        _wingList = [NSArray array];
        
        _radiusCurve = [[KSRCurveGenerator alloc] initWithValue:10.0
                                                  minimumValue:-15.0
                                                  maximumValue:15.0
                                                         wraps:NO
                                               maximumVelocity:0.1
                                           maximumAcceleration:0.01
                                    ticksPerAccelerationChange:150];
        _angleCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                               maximumVelocity:2.0
                                           maximumAcceleration:0.05
                                    ticksPerAccelerationChange:120];
        _deltaAngleCurve = [KSRCurveGenerator curveForAngleWithValue:15.0
                                                    maximumVelocity:0.2
                                                maximumAcceleration:0.02
                                         ticksPerAccelerationChange:80];
        _deltaZCurve = [[KSRCurveGenerator alloc] initWithValue:0.5
                                                  minimumValue:0.4
                                                  maximumValue:0.7
                                                         wraps:NO
                                               maximumVelocity:0.01
                                           maximumAcceleration:0.001
                                    ticksPerAccelerationChange:200];
        _rollCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                              maximumVelocity:1.0
                                          maximumAcceleration:0.25
                                   ticksPerAccelerationChange:80];
        _pitchCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                               maximumVelocity:2.0
                                           maximumAcceleration:0.25
                                    ticksPerAccelerationChange:40];
        _yawCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                             maximumVelocity:1.5
                                         maximumAcceleration:0.25
                                  ticksPerAccelerationChange:50];
        _redCurve = [KSRCurveGenerator curveForColorComponentWithValue:0.0
                                                      maximumVelocity:0.04
                                                  maximumAcceleration:0.01
                                           ticksPerAccelerationChange:95];
        _greenCurve = [KSRCurveGenerator curveForColorComponentWithValue:0.0
                                                        maximumVelocity:0.04
                                                    maximumAcceleration:0.01
                                             ticksPerAccelerationChange:40];
        _blueCurve = [KSRCurveGenerator curveForColorComponentWithValue:0.0
                                                       maximumVelocity:0.04
                                                   maximumAcceleration:0.01
                                            ticksPerAccelerationChange:70];
    }
    
    return self;
}

- (void)parseOpenGLVersion:(NSString *)versionString {
    NSLog(@"%@:parseOpenGLVersion:%@",
          self, versionString);
    
    NSScanner * scanner = [NSScanner scannerWithString:versionString];
    unsigned long long majorVersion;
    if ([scanner scanUnsignedLongLong:&majorVersion]) {
        self.glMajorVersion = [[NSNumber numberWithUnsignedLongLong:majorVersion] unsignedIntValue];
        if ([scanner scanCharactersFromSet:[NSCharacterSet characterSetWithCharactersInString:@"."] intoString:nil]) {
            unsigned long long minorVersion;
            if ([scanner scanUnsignedLongLong:&minorVersion]) {
                self.glMinorVersion = [[NSNumber numberWithUnsignedLongLong:minorVersion] unsignedIntValue];
            }
        }
    }
}

- (BOOL)hasOpenGLVersionMajor:(GLuint)major minor:(GLuint)minor {
    return (self.glMajorVersion > major) || (self.glMajorVersion == major && self.glMinorVersion >= minor);
}

- (void)prepareOpenGL {
    [super prepareOpenGL];
    
    NSLog(@"%@:prepareOpenGL", self);
    
    GLubyte const * const glVersion = glGetString(GL_VERSION);
    [self parseOpenGLVersion:[NSString stringWithCString:(char const *)glVersion
                                                encoding:NSASCIIStringEncoding]];
    
    glEnable(GL_DEPTH_TEST);
    if ([self hasOpenGLVersionMajor:1 minor:1]) {
        glPolygonOffset(-0.5, -2.0);
    }
    
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    
    glEnable(GL_POLYGON_SMOOTH);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    glLoadIdentity();
    // GLKMatrix4MakeLookAt
    gluLookAt(0.0, 50.0, 50.0,
              0.0, 0.0, 13.0,
              0.0, 0.0, 1.0);
    
    self.wingDisplayList = glGenLists(1);
    glNewList(self.wingDisplayList, GL_COMPILE);
    glBegin(GL_QUADS);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glEnd();
    glEndList();
}

- (void)advanceAnimation {
//    NSLog(@"%@:advanceAnimation", self);
    
    [self.openGLContext makeCurrentContext];
    
    GLuint displayList;
    if (self.wingList.count == 0 || self.wingList.count < [KSRWingsView numWings]) {
        displayList = glGenLists(1);
    } else {
        displayList = self.wingList.lastObject.glDisplayList;
        self.wingList = [self.wingList subarrayWithRange:NSMakeRange(0, self.wingList.count - 1)];
    }
    KSRColor *color = [[KSRColor alloc] initWithRed:[self.redCurve getNextValue]
                                              green:[self.greenCurve getNextValue]
                                               blue:[self.blueCurve getNextValue]];
    KSRWing *wing = [[KSRWing alloc] initWithGLDisplayList:displayList
                                                    radius:[self.radiusCurve getNextValue]
                                                     angle:[self.angleCurve getNextValue]
                                                deltaAngle:[self.deltaAngleCurve getNextValue]
                                                    deltaZ:[self.deltaZCurve getNextValue]
                                                      roll:[self.rollCurve getNextValue]
                                                     pitch:[self.pitchCurve getNextValue]
                                                       yaw:[self.yawCurve getNextValue]
                                                     color:color
                                                 edgeColor:[KSRColor white]];
    self.wingList = [[NSArray arrayWithObject:wing] arrayByAddingObjectsFromArray:self.wingList];
    
    glNewList(displayList, GL_COMPILE);
    glPushMatrix();
    glRotatef(wing.angle, 0, 0, 1);
    glTranslatef(wing.radius, 0, 0);
    glRotatef(-wing.yaw, 0, 0, 1);
    glRotatef(-wing.pitch, 0, 1, 0);
    glRotatef(wing.roll, 1, 0, 0);
    glCallList(self.wingDisplayList);
    glPopMatrix();
    glEndList();
    
//    glFlush();
    
    [self setNeedsDisplay:YES];
}

- (void)reshape {
    [super reshape];
    
//    NSLog(@"%@:reshape:bounds:{%f, %f, %f, %f} frame:{%f, %f, %f, %f}",
//          self,
//          self.bounds.origin.x, self.bounds.origin.y, self.bounds.size.width, self.bounds.size.height,
//          self.frame.origin.x, self.frame.origin.y, self.frame.size.width, self.frame.size.height);
    
    [self.openGLContext makeCurrentContext];
    
    NSSize const backingSize = [self convertSizeToBacking:self.bounds.size];
    
    GLdouble xmult = 1.0;
    GLdouble ymult = 1.0;
    if (backingSize.width > backingSize.height) {
        xmult = (GLdouble)(backingSize.width) / (GLdouble)(backingSize.height);
    } else {
        ymult = (GLdouble)(backingSize.height) / (GLdouble)(backingSize.width);
    }
    
    // NSOpenGLView handles calling glViewport.
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20.0 * xmult, 20.0 * xmult,
            -20.0 * ymult, 20.0 * ymult,
            35.0, 105.0);
    glMatrixMode(GL_MODELVIEW);
    
    [self.openGLContext update];
}

- (void)drawRect:(NSRect)dirtyRect {
//    NSLog(@"%@:drawRect:{%f, %f, %f, %f}",
//          self, dirtyRect.origin.x, dirtyRect.origin.y, dirtyRect.size.width, dirtyRect.size.height);
    
    [self.openGLContext makeCurrentContext];
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if ([self hasOpenGLVersionMajor:1 minor:1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPushMatrix();
        for (KSRWing *wing in self.wingList) {
            glTranslatef(0, 0, wing.deltaZ);
            glRotatef(wing.deltaAngle, 0, 0, 1);
            
            KSRColor *color = wing.edgeColor;
            glColor3f(color.red, color.green, color.blue);
            glCallList(wing.glDisplayList);
        }
        glPopMatrix();
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glPushMatrix();
    for (KSRWing *wing in self.wingList) {
        glTranslatef(0, 0, wing.deltaZ);
        glRotatef(wing.deltaAngle, 0, 0, 1);
        
        KSRColor *color = wing.color;
        glColor3f(color.red, color.green, color.blue);
        glCallList(wing.glDisplayList);
    }
    glPopMatrix();
    
//    glFlush();
    
    [self.openGLContext flushBuffer];
}

@end
