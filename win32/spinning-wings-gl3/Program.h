#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <memory>
#include <string>

#include "FragmentShader.h"
#include "VertexShader.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// Manages a GLSL program.  This is composed of shaders.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The invariant established by this class is that the OpenGL program
    /// object is allocated and successfully linked.  The input shaders are
    /// only required for construction.  After the object is constructed, the
    /// shaders are no longer required.
    /// </para>
    /// <para>
    /// This is designed for OpenGL 3.2 or greater.
    /// </para>
    /// </remarks>
    class Program
    {
    public:
        /// <summary>
        /// Default constructor is deleted.  A program is not valid without any shaders.
        /// </summary>
        Program(void) = delete;

        /// <summary>
        /// Creates a GLSL program for transformation feedback.
        /// </summary>
        /// <param name="vertexShader">The vertex shaders to assemble.  Only one may define the <c>main</c> function.</param>
        /// <param name="capturedVaryings">The varying variables to capture.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
            std::initializer_list<std::string> capturedVaryings);

        /// <summary>
        /// Creates a GLSL program for rendering.
        /// </summary>
        /// <param name="vertexShader">The vertex shaders to assemble.  Only one may define the <c>main</c> function.</param>
        /// <param name="fragmentShaders">The fragment shaders to assemble.  Only one may define the <c>main</c> function.</param>
        /// <param name="fragmentData">The fragment shader output variable to be written into the output buffer.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(
            std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
            std::initializer_list<std::shared_ptr<FragmentShader const> > fragmentShaders,
            std::string const& fragmentData);

#pragma region Rule of Five

    public:
        Program(Program const&) = delete;
        Program& operator=(Program const&) = delete;
        Program(Program&&) noexcept = delete;
        Program& operator=(Program&&) noexcept = delete;
        virtual ~Program(void) noexcept;

#pragma endregion

    public:
        /// <summary>
        /// Validate that the GLSL program can execute given the current GL state.
        /// </summary>
        /// <exception cref="std::runtime_error">If not.</exception>
        void Validate(void) const;

        /// <summary>
        /// Returns the OpenGL name for the program object.
        /// </summary>
        /// <returns>The program object name.</returns>
        [[nodiscard]]
        GLuint GetName(void) const noexcept;

        /// <summary>
        /// Returns the index of the generic vertex attribute that is bound to the
        /// specified attribute variable.
        /// </summary>
        /// <param name="attributeName">The name of the attribute.</param>
        /// <returns>The generic attribute index.</returns>
        /// <exception cref="std::runtime_error">If the attribute name is not bound.</exception>
        [[nodiscard]]
        GLuint getAttributeLocation(std::string const& attributeName) const;

        /// <summary>
        /// Returns the location of the specified uniform variable.
        /// </summary>
        /// <param name="uniformName">The name of the uniform variable.</param>
        /// <returns>The uniform location.</returns>
        /// <exception cref="std::runtime_error">If the uniform name is not bound.</exception>
        [[nodiscard]]
        GLint getUniformLocation(std::string const& uniformName) const;

        /// <summary>
        /// Installs the GLSL program as part of the current rendering state.
        /// </summary>
        /// <exception cref="std::runtime_error">If the program could not be installed.</exception>
        void useProgram(void) const;

    private:
        /// <summary>
        /// The OpenGL name for the program object.
        /// </summary>
        GLuint const name{ 0 };

        /// <summary>
        /// The log output from linking the shaders into a program.
        /// </summary>
        std::string linkLog{};
    };

}
