in vec4 vertexPosition;
in vec4 vertexColor;

in uint wingIndex;
in float deltaAngle;
in float deltaZ;

in float radius;
in float angle;
in float roll;
in float pitch;
in float yaw;

smooth out vec4 color;

uniform mat4 modelviewMatrix;

void main() {
    // translate by deltaZ * wingIndex;
    // rotate by deltaAngle * wingIndex;

    // rotate by angle;
    // translate by radius;
    // rotate by yaw, pitch, roll;
    gl_Position = vertexPosition;
    color = vertexColor;
}
