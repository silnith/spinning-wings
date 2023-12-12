#version 120

mat4 translate(in vec3 move) {
    mat4 trans = mat4(1.0);
    trans[3].xyz = move;
    return trans;
}

mat4 rotate(in float angle, in vec3 axis) {
    float c = cos(radians(angle));
    float s = sin(radians(angle));

    mat3 initial = outerProduct(axis, axis) * (1 - c);

    mat3 c_part = mat3(c);

    mat3 s_part = mat3(0, axis.z, -axis.y, -axis.z, 0, axis.x, axis.y, -axis.x, 0) * s;

    mat3 temp = initial + c_part + s_part;
    
    mat4 identity = mat4(1.0);

    mat4 rotation = identity;
    rotation[0].xyz = temp[0];
    rotation[1].xyz = temp[1];
    rotation[2].xyz = temp[2];

    return rotation;
}

void main() {
    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
