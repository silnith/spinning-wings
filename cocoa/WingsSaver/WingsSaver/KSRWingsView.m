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

@interface KSRWingsView ()

@property NSArray<KSRWing *> * wingList;

@property (readonly) KSRCurveGenerator * radiusCurve;
@property (readonly) KSRCurveGenerator * angleCurve;
@property (readonly) KSRCurveGenerator * deltaAngleCurve;
@property (readonly) KSRCurveGenerator * deltaZCurve;
@property (readonly) KSRCurveGenerator * rollCurve;
@property (readonly) KSRCurveGenerator * pitchCurve;
@property (readonly) KSRCurveGenerator * yawCurve;
@property (readonly) KSRCurveGenerator * redCurve;
@property (readonly) KSRCurveGenerator * greenCurve;
@property (readonly) KSRCurveGenerator * blueCurve;

@property GLuint glMajorVersion;
@property GLuint glMinorVersion;

@property GLuint wingDisplayList;

@end

@implementation KSRWingsView

- (instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
    self = [super initWithFrame:frameRect pixelFormat:format];
    
    if (self) {
        [self setFrameOrigin:NSZeroPoint];
        
        GLint swapInterval = 1;
        [self.openGLContext setValues:&swapInterval
                         forParameter:NSOpenGLContextParameterSwapInterval];
        
        _numWings = 40;
        
        _wingList = @[];
        
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
        
        _glMajorVersion = 1;
        _glMinorVersion = 0;
        
        _wingDisplayList = 0;
    }
    
    return self;
}

- (void)parseOpenGLVersion:(NSString *)versionString {
    NSLog(@"%@:parseOpenGLVersion:%@",
          self, versionString);
    
    NSScanner * scanner = [NSScanner scannerWithString:versionString];
    scanner.charactersToBeSkipped = [NSCharacterSet characterSetWithCharactersInString:@""];
    unsigned long long majorVersion;
    if ([scanner scanUnsignedLongLong:&majorVersion]) {
        self.glMajorVersion = [@(majorVersion) unsignedIntValue];
        
        scanner.charactersToBeSkipped = [NSCharacterSet characterSetWithCharactersInString:@"."];
        unsigned long long minorVersion;
        if ([scanner scanUnsignedLongLong:&minorVersion]) {
            self.glMinorVersion = [@(minorVersion) unsignedIntValue];
        }
    }
}

- (BOOL)hasOpenGLVersionMajor:(GLuint)major minor:(GLuint)minor {
    return (self.glMajorVersion > major) || (self.glMajorVersion == major && self.glMinorVersion >= minor);
}

- (void)prepareOpenGL {
    [super prepareOpenGL];
    
    GLubyte const * const glVersion = glGetString(GL_VERSION);
    [self parseOpenGLVersion:@((char const *)glVersion)];
    
    NSLog(@"%@:OpenGL Version:%u.%u",
          self, self.glMajorVersion, self.glMinorVersion);
    
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
    [self.openGLContext makeCurrentContext];
    
    GLuint displayList;
    if (self.wingList.count == 0 || self.wingList.count < self.numWings) {
        displayList = glGenLists(1);
    } else {
        displayList = self.wingList.lastObject.glDisplayList;
//        NSUInteger const numWings = MIN(self.wingList.count, self.numWings);
        self.wingList = [self.wingList subarrayWithRange:NSMakeRange(0, self.wingList.count - 1)];
    }
    KSRColor * color = [KSRColor colorWithRed:[self.redCurve getNextValue]
                                        green:[self.greenCurve getNextValue]
                                         blue:[self.blueCurve getNextValue]];
    KSRWing * wing = [[KSRWing alloc] initWithGLDisplayList:displayList
                                                     radius:[self.radiusCurve getNextValue]
                                                      angle:[self.angleCurve getNextValue]
                                                 deltaAngle:[self.deltaAngleCurve getNextValue]
                                                     deltaZ:[self.deltaZCurve getNextValue]
                                                       roll:[self.rollCurve getNextValue]
                                                      pitch:[self.pitchCurve getNextValue]
                                                        yaw:[self.yawCurve getNextValue]
                                                      color:color
                                                  edgeColor:[KSRColor white]];
    self.wingList = [@[wing] arrayByAddingObjectsFromArray:self.wingList];
//    self.wingList = [[NSArray arrayWithObject:wing] arrayByAddingObjectsFromArray:self.wingList];
    
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
    
    [self setNeedsDisplay:YES];
}

- (void)reshape {
    [super reshape];
    
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
    [self.openGLContext makeCurrentContext];
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if ([self hasOpenGLVersionMajor:1 minor:1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPushMatrix();
        for (KSRWing const * wing in self.wingList) {
            glTranslatef(0, 0, wing.deltaZ);
            glRotatef(wing.deltaAngle, 0, 0, 1);
            
            KSRColor const * color = wing.edgeColor;
            glColor3f(color.red, color.green, color.blue);
            glCallList(wing.glDisplayList);
        }
        glPopMatrix();
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glPushMatrix();
    for (KSRWing const * wing in self.wingList) {
        glTranslatef(0, 0, wing.deltaZ);
        glRotatef(wing.deltaAngle, 0, 0, 1);
        
        KSRColor const * color = wing.color;
        glColor3f(color.red, color.green, color.blue);
        glCallList(wing.glDisplayList);
    }
    glPopMatrix();
    
    [self.openGLContext flushBuffer];
}

@end
