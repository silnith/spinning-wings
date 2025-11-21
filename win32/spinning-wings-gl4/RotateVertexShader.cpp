#include <Windows.h>
#include <GL/glew.h>

#include <string>

#include "RotateVertexShader.h"
#include "VertexShader.h"
#include "Shader.h"

namespace silnith::wings::gl4
{

    RotateVertexShader::RotateVertexShader(void)
        : VertexShader{
            std::initializer_list<std::string>{
                Shader::versionDeclaration,
                R"shaderText(
mat4 rotate(const in float angle, const in vec3 axis) {
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
)shaderText",
            }
        }
    {}

    std::string RotateVertexShader::GetDeclaration(void) const
    {
        return R"shaderText(
mat4 rotate(const in float angle, const in vec3 axis);
)shaderText";
    }

}
