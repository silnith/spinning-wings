#version 120

void main() {
    gl_FragColor = gl_Color;
    gl_FragDepth = gl_FragCoord.z;
}
