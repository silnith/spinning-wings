#version 120
// Shader version 1.20 corresponds to OpenGL 2.1

attribute vec2 deltaZ;
attribute vec2 radiusAngle;
attribute vec3 rollPitchYaw;

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);
mat4 scale(in vec3 factor);

/*
 * The shader entry point.
 * Vertex shaders may read gl_Color, gl_SecondaryColor,
 * gl_Normal, gl_Vertex, gl_FogCoord,
 * and a bunch of texture coordinate stuff.
 * 
 * A vertex shader must write to gl_Position.
 * It may write to gl_PointSize and gl_ClipVertex.
 */
void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float deltaAngle = deltaZ[0];
    float dZ = deltaZ[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    /*
     * This implements the additional transformations
     * that were applied in the wing-rendering loop.
     * These are relative to the position of the wing in the list.
     * Specifically:
     * glTranslatef(0, 0, wing.getDeltaZ());
     * glRotatef(wing.getDeltaAngle(), 0, 0, 1);
     */
    mat4 deltaTransformation = translate(vec3(0, 0, dZ))
                               * rotate(deltaAngle, zAxis);
    /*
     * This implements the transformations that were handled by the
     * wing-specific display list in the OpenGL 1.0 version.
     * These are consistent for the lifetime of a wing.
     * Specifically:
     * glRotatef(angle, 0, 0, 1);
     * glTranslatef(radius, 0, 0);
     * glRotatef(-yaw, 0, 0, 1);
     * glRotatef(-pitch, 0, 1, 0);
     * glRotatef(roll, 1, 0, 0);
     */
    mat4 wingTransformation = rotate(angle, zAxis)
                              * translate(vec3(radius, 0, 0))
                              * rotate(-yaw, zAxis)
                              * rotate(-pitch, yAxis)
                              * rotate(roll, xAxis);

    /*
     * The OpenGL 1.0 version did not use face culling.
     * 
     * gl_Color is a built-in attribute.
     * gl_FrontColor and gl_BackColor are built-in varying variables.
     */
    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
    /*
     * A vertex shader must set the gl_Position variable.
     * gl_ModelViewProjectionMatrix is a built-in uniform.
     * gl_Vertex is a built-in attribute.
     */
    gl_Position = gl_ModelViewProjectionMatrix * deltaTransformation * wingTransformation * gl_Vertex;
}

mat4 rotate(in float angle, in vec3 axis) {
    // OpenGL has always specified angles in degrees.
    // Trigonometric functions operate on radians.
    float c = cos(radians(angle));
    float s = sin(radians(angle));

    mat3 initial = outerProduct(axis, axis)
                   * (1 - c);
    mat3 c_part = mat3(c);
    mat3 s_part = mat3(0, axis.z, -axis.y,
                       -axis.z, 0, axis.x,
                       axis.y, -axis.x, 0)
                  * s;
    mat3 temp = initial + c_part + s_part;

    mat4 rotation = mat4(1.0);
    rotation[0].xyz = temp[0];
    rotation[1].xyz = temp[1];
    rotation[2].xyz = temp[2];

    return rotation;
}

mat4 translate(in vec3 move) {
    mat4 trans = mat4(1.0);
    trans[3].xyz = move;
    return trans;
}

mat4 scale(in vec3 factor) {
    mat4 result;
    result[0][0] = factor.x;
    result[1][1] = factor.y;
    result[2][2] = factor.z;
    result[3][3] = 1;
    return result;
}
