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

namespace silnith::wings::gl4
{

    std::string const Shader::versionDeclaration{
        R"shaderText(#version 410 core
// Shader version 4.10 corresponds to OpenGL 4.1
)shaderText"
    };

    std::string const Shader::rotateMatrixFunctionDeclaration{
        R"shaderText(
mat4 rotate(const in float angle, const in vec3 axis);
)shaderText"
    };

    std::string const Shader::rotateMatrixFunctionDefinition{
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
)shaderText"
    };

    std::string const Shader::translateMatrixFunctionDeclaration{
        R"shaderText(
mat4 translate(const in vec3 move);
)shaderText"
    };

    std::string const Shader::translateMatrixFunctionDefinition{
        R"shaderText(
mat4 translate(const in vec3 move) {
    mat4 trans = mat4(1.0);
    trans[3].xyz = move;
    return trans;
}
)shaderText"
    };

    std::string const Shader::scaleMatrixFunctionDeclaration{
        R"shaderText(
mat4 scale(const in vec3 factor);
)shaderText"
    };

    std::string const Shader::scaleMatrixFunctionDefinition{
        R"shaderText(
mat4 scale(const in vec3 factor) {
    mat4 result;
    result[0][0] = factor.x;
    result[1][1] = factor.y;
    result[2][2] = factor.z;
    result[3][3] = 1;
    return result;
}
)shaderText"
    };

    Shader::Shader(GLenum type, std::initializer_list<std::string> const& sources)
        : name{ glCreateShader(type) }, compilationLog{}
    {
        /*
         * OpenGL 4.1 defines compute shaders, vertex shaders,
         * tessellation shaders (control and evaluation),
         * geometry shaders, and fragment shaders.
         */
        assert((type == GL_COMPUTE_SHADER)
            || (type == GL_VERTEX_SHADER)
            || (type == GL_TESS_CONTROL_SHADER)
            || (type == GL_TESS_EVALUATION_SHADER)
            || (type == GL_GEOMETRY_SHADER)
            || (type == GL_FRAGMENT_SHADER));

        if (name == 0)
        {
            throw std::runtime_error{ "Failed to create shader object." };
        }

        /*
         * In order to create a GLSL shader, source code must be provided as
         * as strings.
         */
        {
            /*
             * Since this is largely demonstration code, I have gone out of my
             * way to make the variable types explicit and precise.
             */
            std::vector<GLchar const*> cSources{};
            cSources.reserve(sources.size());
            for (std::string const& source : sources)
            {
                cSources.emplace_back(source.c_str());
            }
            GLsizei const cSourcesSize{ static_cast<GLsizei>(cSources.size()) };
            glShaderSource(name, cSourcesSize, cSources.data(), nullptr);
            /*
             * The call to glShaderSource copies the strings into GL memory.
             */
        }

        glCompileShader(name);

        /*
         * Whether the compilation succeeded or failed, a log may be created.
         * The length of the log must be queried in order to know how much
         * buffer space is needed to retrieve the log.  The length will
         * include the terminating null character.  If the length is zero,
         * there is nothing in the log.
         */
        GLint logSize{ 0 };
        glGetShaderiv(name, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetShaderInfoLog(name, static_cast<GLsizei>(logSize), nullptr, log.get());
            compilationLog = std::string{ log.get() };
        }

        /*
         * The GLSL shader may not have compiled correctly.  If compilation
         * failed, the shader is not valid and cannot be used.
         */
        GLint compilationSuccess{ 0 };
        glGetShaderiv(name, GL_COMPILE_STATUS, &compilationSuccess);
        switch (compilationSuccess)
        {
        case GL_TRUE:
            break;
        case GL_FALSE:
        {
            /*
             * Since throwing an exception cancels construction of the C++
             * object, the destructor will never be called.  Therefore we need
             * to clean up the allocated shader before throwing.
             */
            glDeleteShader(name);
            throw std::runtime_error{ compilationLog };
        }
        default:
        {
            /*
             * The compilation status will either be GL_TRUE or GL_FALSE, so
             * this case will never execute on a conforming OpenGL
             * implementation.  But the C++ compiler has no way to prove that.
             */
            glDeleteShader(name);
            std::ostringstream errorMessage{ "Unknown compilation status: "s };
            errorMessage << compilationSuccess;
            throw std::runtime_error{ errorMessage.str() };
        }
        }
    }

    Shader::~Shader(void) noexcept
    {
        /*
         * Passing zero to the delete function will be silently ignored.
         */
        glDeleteShader(name);
    }

    GLuint Shader::GetName(void) const noexcept
    {
        return name;
    }

}
