#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

#include "TranslateVertexShader.h"
#include "VertexShader.h"
#include "Shader.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl4
{

    /// <summary>
    /// The GLSL function declaration for <c>translate</c>, which returns a
    /// transformation matrix that translates.
    /// </summary>
    static std::string const translateMatrixFunctionDeclaration{
        R"shaderText(
mat4 translate(const in vec3 move);
)shaderText"
    };

    /// <summary>
    /// The GLSL function definition for <c>translate</c>, which returns a
    /// transformation matrix that translates.
    /// </summary>
    static std::string const translateMatrixFunctionDefinition{
        R"shaderText(
mat4 translate(const in vec3 move) {
    mat4 trans = mat4(1.0);
    trans[3].xyz = move;
    return trans;
}
)shaderText"
    };

    TranslateVertexShader::TranslateVertexShader(void)
        : VertexShader{
            Shader::versionDeclaration,
            translateMatrixFunctionDefinition,
        }
    {}

    std::string TranslateVertexShader::GetDeclaration(void) const
    {
        return translateMatrixFunctionDeclaration;
    }

}
