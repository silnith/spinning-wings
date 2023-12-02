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

+ (GLsizei)numWings {
    return 40;
}

- (instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
    self = [super initWithFrame:frameRect pixelFormat:format];
    
    if (self) {
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
        
        _wingList = [NSArray array];
    }
    
    return self;
}

- (BOOL)hasOpenGLVersionMajor:(GLuint)major minor:(GLuint)minor {
    return YES;
}

- (void)prepareOpenGL {
    [super prepareOpenGL];
    
    NSLog(@"Preparing OpenGL context.");
    
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
    
    _wingDisplayList = glGenLists(1);
    glNewList(_wingDisplayList, GL_COMPILE);
    glBegin(GL_QUADS);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glEnd();
    glEndList();
    
    _wingDisplayLists = glGenLists([KSRWingsView numWings]);
    for (GLuint displayList = _wingDisplayLists;
         displayList < _wingDisplayLists + [KSRWingsView numWings];
         displayList++) {
        _wingList = [_wingList arrayByAddingObject:[[KSRWing alloc] initWithGLDisplayList:displayList]];
        glNewList(displayList, GL_COMPILE);
        glEndList();
    }
    
    NSLog(@"Prepared OpenGL context.");
}

- (void)clearGLContext {
    NSLog(@"Clearing OpenGL context.");
    
    _wingList = [NSArray array];
    glDeleteLists(_wingDisplayLists, [KSRWingsView numWings]);
    _wingDisplayLists = 0;
    glDeleteLists(_wingDisplayList, 1);
    _wingDisplayList = 0;
    
    NSLog(@"Cleared OpenGL context.");
    
    [super clearGLContext];
}

- (void)advanceAnimation {
    NSLog(@"Advancing animation.");
    
    NSAssert(_wingList.count == [KSRWingsView numWings],
             @"List of wings length is incorrect.  Expected: %d, Actual: %lu",
             [KSRWingsView numWings], (unsigned long) _wingList.count);
    
    GLuint const displayList = _wingList.lastObject.glDisplayList;
    NSRange range = NSMakeRange(1, _wingList.count - 1);
    _wingList = [_wingList subarrayWithRange:range];
    KSRColor *color = [[KSRColor alloc] initWithRed:[_redCurve getNextValue]
                                              green:[_greenCurve getNextValue]
                                               blue:[_blueCurve getNextValue]];
    KSRWing *wing = [[KSRWing alloc] initWithGLDisplayList:displayList
                                                    radius:[_radiusCurve getNextValue]
                                                     angle:[_angleCurve getNextValue]
                                                deltaAngle:[_deltaAngleCurve getNextValue]
                                                    deltaZ:[_deltaZCurve getNextValue]
                                                      roll:[_rollCurve getNextValue]
                                                     pitch:[_pitchCurve getNextValue]
                                                       yaw:[_yawCurve getNextValue]
                                                     color:color
                                                 edgeColor:[KSRColor white]];
    _wingList = [_wingList arrayByAddingObject:wing];
    
//     TODO: Do I need this?
    [self.openGLContext makeCurrentContext];
    
    glNewList(displayList, GL_COMPILE);
    glPushMatrix();
    glRotatef(wing.angle, 0, 0, 1);
    glTranslatef(wing.radius, 0, 0);
    glRotatef(-wing.yaw, 0, 0, 1);
    glRotatef(-wing.pitch, 0, 1, 0);
    glRotatef(wing.roll, 1, 0, 0);
    glCallList(_wingDisplayList);
    glPopMatrix();
    glEndList();
    
    // TODO: Do I need this?
    [self.openGLContext update];
    
    [self setNeedsDisplay:YES];
    
    NSLog(@"Advanced animation.");
}

- (void)update {
    [super update];
    
    NSLog(@"Updating");
    
    NSLog(@"Updated");
}

- (void)reshape {
    [super reshape];
    
    NSLog(@"Reshaping");
    
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLdouble xmult = 1.0;
    GLdouble ymult = 1.0;
    if (backingBounds.size.width > backingBounds.size.height) {
        xmult = (GLdouble)(backingBounds.size.width) / (GLdouble)(backingBounds.size.height);
    } else {
        ymult = (GLdouble)(backingBounds.size.height) / (GLdouble)(backingBounds.size.width);
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20.0 * xmult, 20.0 * xmult,
            -20.0 * ymult, 20.0 * ymult,
            35.0, 105.0);
    glMatrixMode(GL_MODELVIEW);
    
    NSLog(@"Reshaped");
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    NSLog(@"Drawing");
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if ([self hasOpenGLVersionMajor:1 minor:1]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPushMatrix();
        for (KSRWing *wing in _wingList) {
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
    for (KSRWing *wing in _wingList) {
        glTranslatef(0, 0, wing.deltaZ);
        glRotatef(wing.deltaAngle, 0, 0, 1);
        
        KSRColor *color = wing.color;
        glColor3f(color.red, color.green, color.blue);
        glCallList(wing.glDisplayList);
    }
    glPopMatrix();
    
    glFlush();
    
    NSLog(@"Drawn");
}

@end
