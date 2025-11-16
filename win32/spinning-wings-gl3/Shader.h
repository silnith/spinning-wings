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
    /// The invariant established by this class is that the shader is allocated
    /// and successfully compiled from the source code.  The source code
    /// strings are only required for the constructor, once the object is
    /// constructed the input strings are no longer needed.
    /// </para>
    /// <para>
    /// This is designed for OpenGL 3.2 or greater.
    /// </para>
    /// </remarks>
    class Shader
    {
#pragma region Static Members

    public:
        /// <summary>
        /// The GLSL 1.50 version declaration corresponding to OpenGL 3.2.
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

#pragma endregion

    public:
        /// <summary>
        /// Default constructor is deleted.  A shader is not valid without source code.
        /// </summary>
        Shader(void) = delete;

    protected:
        /// <summary>
        /// Creates and compiles a shader from the given GLSL sources.
        /// The source strings are concatenated.
        /// </summary>
        /// <param name="type">The type of shader.  This should be one of
        /// <c>GL_VERTEX_SHADER</c>, <c>GL_GEOMETRY_SHADER</c>, or <c>GL_FRAGMENT_SHADER</c>.</param>
        /// <param name="sources">The source strings to concatenate.</param>
        /// <exception cref="std::runtime_error">If an error occurs creating the shader object in the OpenGL state machine.</exception>
        explicit Shader(GLenum type, std::initializer_list<std::string> sources);

#pragma region Rule of Five

    public:
        Shader(Shader const&) = delete;
        Shader& operator=(Shader const&) = delete;
        Shader(Shader&&) noexcept = delete;
        Shader& operator=(Shader&&) noexcept = delete;
        virtual ~Shader(void) noexcept;

#pragma endregion

    public:
        /// <summary>
        /// Returns the OpenGL name for the shader object.
        /// </summary>
        /// <returns>The shader object name.</returns>
        [[nodiscard]]
        GLuint GetName(void) const noexcept;

    private:
        /// <summary>
        /// The OpenGL name for the shader object.
        /// </summary>
        GLuint const name{ 0 };

        /// <summary>
        /// The log output from compiling the shader.
        /// </summary>
        std::string compilationLog{};
    };

}
