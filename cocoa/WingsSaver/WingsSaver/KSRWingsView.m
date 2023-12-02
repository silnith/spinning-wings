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

- (instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
    self = [super initWithFrame:frameRect pixelFormat:format];
    
    if (self) {
        radiusCurve = [[KSRCurveGenerator alloc] initWithValue:10.0
                                                  minimumValue:-15.0
                                                  maximumValue:15.0
                                                         wraps:NO
                                               maximumVelocity:0.1
                                           maximumAcceleration:0.01
                                    ticksPerAccelerationChange:150];
        angleCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                               maximumVelocity:2.0
                                           maximumAcceleration:0.05
                                    ticksPerAccelerationChange:120];
        deltaAngleCurve = [KSRCurveGenerator curveForAngleWithValue:15.0
                                                    maximumVelocity:0.2
                                                maximumAcceleration:0.02
                                         ticksPerAccelerationChange:80];
        deltaZCurve = [[KSRCurveGenerator alloc] initWithValue:0.5
                                                  minimumValue:0.4
                                                  maximumValue:0.7
                                                         wraps:NO
                                               maximumVelocity:0.01
                                           maximumAcceleration:0.001
                                    ticksPerAccelerationChange:200];
        rollCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                              maximumVelocity:1.0
                                          maximumAcceleration:0.25
                                   ticksPerAccelerationChange:80];
        pitchCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                               maximumVelocity:2.0
                                           maximumAcceleration:0.25
                                    ticksPerAccelerationChange:40];
        yawCurve = [KSRCurveGenerator curveForAngleWithValue:0.0
                                             maximumVelocity:1.5
                                         maximumAcceleration:0.25
                                  ticksPerAccelerationChange:50];
        redCurve = [KSRCurveGenerator curveForColorComponentWithValue:0.0
                                                      maximumVelocity:0.04
                                                  maximumAcceleration:0.01
                                           ticksPerAccelerationChange:95];
        greenCurve = [KSRCurveGenerator curveForColorComponentWithValue:0.0
                                                        maximumVelocity:0.04
                                                    maximumAcceleration:0.01
                                             ticksPerAccelerationChange:40];
        blueCurve = [KSRCurveGenerator curveForColorComponentWithValue:0.0
                                                       maximumVelocity:0.04
                                                   maximumAcceleration:0.01
                                            ticksPerAccelerationChange:70];
    }
    
    return self;
}

- (BOOL)hasOpenGLVersionMajor:(GLuint)major minor:(GLuint)minor {
    return YES;
}

- (void)prepareOpenGL {
    [super prepareOpenGL];
    
    NSLog(@"Preparing OpenGL context.");
    
    // TODO: Do I need this?
//    [self.openGLContext makeCurrentContext];
    
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
    gluLookAt(0.0, 50.0, 50.0,
              0.0, 0.0, 13.0,
              0.0, 0.0, 1.0);
    // GLKMatrix4MakeLookAt
    
    wingDisplayList = glGenLists(1);
    glNewList(wingDisplayList, GL_COMPILE);
    glBegin(GL_QUADS);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glEnd();
    glEndList();
    
    wingList = [NSArray array];
    wingDisplayLists = glGenLists(numWings);
    for (GLuint displayList = wingDisplayLists;
         displayList < wingDisplayLists + numWings;
         displayList++) {
        wingList = [wingList arrayByAddingObject:[[KSRWing alloc] initWithGLDisplayList:displayList]];
        glNewList(displayList, GL_COMPILE);
        glEndList();
    }
    
    NSLog(@"Prepared OpenGL context.");
    
    // TODO: Do I need this?
//    [self.openGLContext update];
}

- (void)clearGLContext {
    NSLog(@"Clearing OpenGL context.");
    
    wingList = nil;
    glDeleteLists(wingDisplayLists, numWings);
    glDeleteLists(wingDisplayList, 1);
    
    NSLog(@"Cleared OpenGL context.");
    
    [super clearGLContext];
}

- (void)advanceAnimation {
    NSLog(@"Advancing animation.");
    
    NSAssert(wingList.count == numWings,
             @"List of wings length is incorrect.  Expected: %d, Actual: %lu",
             numWings, (unsigned long) wingList.count);
    GLuint displayList = wingList.lastObject.glDisplayList;
    NSRange range = NSMakeRange(1, wingList.count - 1);
    wingList = [wingList subarrayWithRange:range];
    KSRWing *wing = [[KSRWing alloc] initWithGLDisplayList:displayList
                                                    radius:[radiusCurve getNextValue]
                                                     angle:[angleCurve getNextValue]
                                                deltaAngle:[deltaAngleCurve getNextValue]
                                                    deltaZ:[deltaZCurve getNextValue]
                                                      roll:[rollCurve getNextValue]
                                                     pitch:[pitchCurve getNextValue]
                                                       yaw:[yawCurve getNextValue]
                                                     color:[[KSRColor alloc] initWithRed:[redCurve getNextValue] green:[greenCurve getNextValue] blue:[blueCurve getNextValue]]
                                                 edgeColor:[KSRColor white]];
    wingList = [wingList arrayByAddingObject:wing];
    
    // TODO: Do I need this?
//    [self.openGLContext makeCurrentContext];
    
    glNewList(displayList, GL_COMPILE);
    glPushMatrix();
    glRotatef(wing.angle, 0, 0, 1);
    glTranslatef(wing.radius, 0, 0);
    glRotatef(-wing.yaw, 0, 0, 1);
    glRotatef(-wing.pitch, 0, 1, 0);
    glRotatef(wing.roll, 1, 0, 0);
    glCallList(wingDisplayList);
    glPopMatrix();
    glEndList();
    
    // TODO: Do I need this?
//    [self.openGLContext update];
    
    [self setNeedsDisplay:YES];
    
    NSLog(@"Advanced animation.");
}

- (void)reshape {
    // TODO: Do I need this?
//    [self.openGLContext makeCurrentContext];
    
    NSLog(@"Reshaping");
    
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLdouble xmult = 1.0;
    GLdouble ymult = 1.0;
    if (backingBounds.size.width > backingBounds.size.height) {
        xmult = (GLdouble)(backingBounds.size.width) / (GLdouble)(backingBounds.size.height);
    } else {
        ymult = (GLdouble)(backingBounds.size.height) / (GLdouble)(backingBounds.size.width);
    }
    
//    GLint const x = 0;
//    GLint const y = 0;
//    GLsizei const width = backingBounds.size.width;
//    GLsizei const height = backingBounds.size.height;
    
    // TODO: Do I need this?
//    glViewport(x, y, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20.0 * xmult, 20.0 * xmult,
            -20.0 * ymult, 20.0 * ymult,
            35.0, 105.0);
    glMatrixMode(GL_MODELVIEW);
    
    // TODO: Do I need this?
//    [self.openGLContext update];
    
    NSLog(@"Reshaped");
}

- (void)drawRect:(NSRect)dirtyRect {
//    [super drawRect:dirtyRect];
    
    NSLog(@"Drawing");
    
    // TODO: Do I need this?
//    [self.openGLContext makeCurrentContext];
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if ([self hasOpenGLVersionMajor:1 minor:1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPushMatrix();
        for (KSRWing *wing in wingList) {
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
    for (KSRWing *wing in wingList) {
        glTranslatef(0, 0, wing.deltaZ);
        glRotatef(wing.deltaAngle, 0, 0, 1);
        
        KSRColor *color = wing.edgeColor;
        glColor3f(color.red, color.green, color.blue);
        glCallList(wing.glDisplayList);
    }
    glPopMatrix();
    
    glFlush();
    
    NSLog(@"Drawn");
    
    // TODO: Do I need this?
//    [self.openGLContext flushBuffer];
//    [self.openGLContext update];
}

@end
