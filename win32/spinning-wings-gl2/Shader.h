#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

namespace silnith::wings::gl2
{
    /// <summary>
    /// Manages a single GLSL shader.  Use the <c>FragmentShader</c>
    /// or <c>VertexShader</c> specializations instead.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
    /// </para>
    /// </remarks>
    class Shader
    {
    public:
        /// <summary>
        /// The GLSL 1.20 version declaration corresponding to OpenGL 2.1.
        /// This needs to be the first non-whitespace part of the shader
        /// source code.
        /// </summary>
        /// <remarks>
        /// <para>
        /// If the version declaration is omitted, the source code will be
        /// assumed to be GLSL 1.10, corresponding to OpenGL 2.0.
        /// </para>
        /// </remarks>
        static std::string const versionDeclaration;

        /// <summary>
        /// The GLSL function declaration for <c>rotate</c>, which returns a
        /// transformation matrix that rotates around an axis.
        /// </summary>
        static std::string const rotateMatrixFunctionDeclaration;

        /// <summary>
        /// The GLSL function definition for <c>rotate</c>, which returns a
        /// transformation matrix that rotates around an axis.
        /// </summary>
        static std::string const rotateMatrixFunctionDefinition;

        /// <summary>
        /// The GLSL function declaration for <c>translate</c>, which returns a
        /// transformation matrix that translates.
        /// </summary>
        static std::string const translateMatrixFunctionDeclaration;

        /// <summary>
        /// The GLSL function definition for <c>translate</c>, which returns a
        /// transformation matrix that translates.
        /// </summary>
        static std::string const translateMatrixFunctionDefinition;

        /// <summary>
        /// The GLSL function declaration for <c>scale</c>, which returns a
        /// transformation matrix that scales.
        /// </summary>
        static std::string const scaleMatrixFunctionDeclaration;

        /// <summary>
        /// The GLSL function definition for <c>scale</c>, which returns a
        /// transformation matrix that scales.
        /// </summary>
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
        /// The source strings are concatenated.
        /// </summary>
        /// <param name="type">The type of shader.  This should be one of <c>GL_VERTEX_SHADER</c> or <c>GL_FRAGMENT_SHADER</c>.</param>
        /// <param name="sources">The source strings to concatenate.</param>
        /// <exception cref="std::runtime_error">If an error occurs creating the shader object in the OpenGL state machine.</exception>
        explicit Shader(GLenum type, std::initializer_list<std::string> sources);

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
