#version 120
// Shader version 1.20 corresponds to OpenGL 2.1

/*
 * The shader entry point.
 * Fragment shaders may read the variables gl_FragCoord and gl_FrontFacing.
 */
void main() {
    /*
     * gl_Color is a built-in varying variable based on the gl_FrontColor
     * or gl_BackColor written in the vertex shader.
     * 
     * A fragment shader must write to gl_FragColor if the render target uses colors.
     * gl_FragDepth must be written if the depth buffer is enabled.
     * If gl_FragDepth is not written, it will take the value of gl_FragCoord.z.
     */
    gl_FragColor = gl_Color;
    gl_FragDepth = gl_FragCoord.z;
}
