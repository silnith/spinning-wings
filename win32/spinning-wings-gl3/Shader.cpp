#include <Windows.h>
#include <GL/glew.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cassert>
#include <cstddef>

#include "Shader.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl3
{

    std::string const Shader::versionDeclaration{
        R"shaderText(#version 150 core
// Shader version 1.50 corresponds to OpenGL 3.2

layout(shared, column_major) uniform;
)shaderText"
    };

    std::string const Shader::rotateMatrixFunctionDeclaration{
        R"shaderText(
mat4 rotate(in float angle, in vec3 axis);
)shaderText"
    };

    std::string const Shader::rotateMatrixFunctionDefinition{
        R"shaderText(
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
)shaderText"
    };

    std::string const Shader::translateMatrixFunctionDeclaration{
        R"shaderText(
mat4 translate(in vec3 move);
)shaderText"
    };

    std::string const Shader::translateMatrixFunctionDefinition{
        R"shaderText(
mat4 translate(in vec3 move) {
    mat4 trans = mat4(1.0);
    trans[3].xyz = move;
    return trans;
}
)shaderText"
    };

    std::string const Shader::scaleMatrixFunctionDeclaration{
        R"shaderText(
mat4 scale(in vec3 factor);
)shaderText"
    };

    std::string const Shader::scaleMatrixFunctionDefinition{
        R"shaderText(
mat4 scale(in vec3 factor) {
    mat4 result;
    result[0][0] = factor.x;
    result[1][1] = factor.y;
    result[2][2] = factor.z;
    result[3][3] = 1;
    return result;
}
)shaderText"
    };

    Shader::Shader(GLenum type, std::initializer_list<std::string> sources)
        : id{ glCreateShader(type) },
        compilationLog{}
    {
        assert((type == GL_VERTEX_SHADER)
            || (type == GL_GEOMETRY_SHADER)
            || (type == GL_FRAGMENT_SHADER));

        if (id == 0)
        {
            throw std::runtime_error{ "Failed to allocate shader." };
        }

        // Limit scope of temporary variables.
        {
            std::vector<GLchar const*> cSources{};
            cSources.reserve(sources.size());
            for (std::string const& source : sources)
            {
                cSources.emplace_back(source.c_str());
            }
            GLsizei const cSourcesSize{ static_cast<GLsizei>(cSources.size()) };
            glShaderSource(id, cSourcesSize, cSources.data(), nullptr);
        }

        glCompileShader(id);

        GLint logSize{ 0 };
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetShaderInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log.get());
            compilationLog = std::string{ log.get() };
        }

        GLint compilationSuccess{ 0 };
        glGetShaderiv(id, GL_COMPILE_STATUS, &compilationSuccess);
        switch (compilationSuccess)
        {
        case GL_TRUE:
            break;
        case GL_FALSE:
        {
            glDeleteShader(id);
            throw std::runtime_error{ compilationLog };
        }
        default:
        {
            glDeleteShader(id);
            std::ostringstream errorMessage{ "Unknown compilation status: "s };
            errorMessage << compilationSuccess;
            throw std::runtime_error{ errorMessage.str() };
        }
        }
    }

    Shader::~Shader(void) noexcept
    {
        glDeleteShader(id);
    }

    GLuint Shader::getShader(void) const noexcept
    {
        return id;
    }

}
