#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

namespace silnith::wings::gl3
{
    /// <summary>
    /// Manages a single GLSL shader.  Use <c>FragmentShader</c>
    /// or <c>VertexShader</c> instead.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 3.2 or greater.
    /// </para>
    /// </remarks>
    class Shader
    {
    public:
        static std::string const rotateMatrixFunctionDeclaration;

        static std::string const rotateMatrixFunctionDefinition;

        static std::string const translateMatrixFunctionDeclaration;

        static std::string const translateMatrixFunctionDefinition;

        static std::string const scaleMatrixFunctionDeclaration;

        static std::string const scaleMatrixFunctionDefinition;

    public:
        Shader(void) noexcept = delete;
        Shader(Shader const&) noexcept = delete;
        Shader& operator=(Shader const&) noexcept = delete;
        Shader(Shader&&) noexcept = delete;
        Shader& operator=(Shader&&) noexcept = delete;
        virtual ~Shader(void) noexcept;
    protected:
        /// <summary>
        /// Creates and compiles a shader from the given GLSL sources.
        /// The source strings are concatenated with newlines appended to each.
        /// </summary>
        /// <param name="type">The type of shader.  This should be one of
        /// <c>GL_VERTEX_SHADER</c>, <c>GL_GEOMETRY_SHADER</c>, or <c>GL_FRAGMENT_SHADER</c>.</param>
        /// <param name="sources">The source strings to concatenate.</param>
        /// <exception cref="std::runtime_error">If an error occurs creating the shader object in the OpenGL state machine.</exception>
        explicit Shader(GLenum type, std::initializer_list<std::string> const&);

    public:
        /// <summary>
        /// Returns the OpenGL name for the shader.
        /// </summary>
        /// <returns>The OpenGL identifier of the shader.</returns>
        [[nodiscard]]
        GLuint getShader(void) const noexcept;

    private:
        GLuint const id{ 0 };
        std::string compilationLog{};
    };

}
