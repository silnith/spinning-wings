#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
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
    /// This is designed for OpenGL 3.2 or greater.
    /// </para>
    /// </remarks>
    class Program
    {
    public:
        Program(void) noexcept = delete;
        Program(Program const&) noexcept = delete;
        Program& operator=(Program const&) noexcept = delete;
        Program(Program&&) noexcept = delete;
        Program& operator=(Program&&) noexcept = delete;
        virtual ~Program(void) noexcept;
    public:
        /// <summary>
        /// Creates a GLSL program for transformation feedback.
        /// </summary>
        /// <param name="vertexShader">The vertex shaders to assemble.  Only one may define the <c>main</c> function.</param>
        /// <param name="capturedVaryings">The varying variables to capture.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(std::initializer_list<std::shared_ptr<VertexShader const> > const& vertexShaders,
            std::initializer_list<std::string> const& capturedVaryings);

        /// <summary>
        /// Creates a GLSL program for rendering.
        /// </summary>
        /// <param name="vertexShader">The vertex shaders to assemble.  Only one may define the <c>main</c> function.</param>
        /// <param name="fragmentShaders">The fragment shaders to assemble.  Only one may define the <c>main</c> function.</param>
        /// <param name="fragmentData">The fragment shader output variable to be written into the output buffer.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(
            std::initializer_list<std::shared_ptr<VertexShader const> > const& vertexShaders,
            std::initializer_list<std::shared_ptr<FragmentShader const> > const& fragmentShaders,
            std::string const& fragmentData);

        /// <summary>
        /// Validate that the GLSL program can execute given the current GL state.
        /// </summary>
        /// <exception cref="std::runtime_error">If not.</exception>
        void Validate(void) const;

        /// <summary>
        /// Returns the OpenGL name for the program.
        /// </summary>
        /// <returns>The OpenGL identifier for the program.</returns>
        [[nodiscard]]
        GLuint getProgram(void) const noexcept;

        /// <summary>
        /// Returns the index of the generic vertex attribute that is bound to the
        /// specified attribute variable.
        /// </summary>
        /// <param name="name">The name of the attribute.</param>
        /// <returns>The generic attribute index.</returns>
        /// <exception cref="std::runtime_error">If the attribute name is not bound.</exception>
        [[nodiscard]]
        GLuint getAttributeLocation(std::string const& name) const;

        /// <summary>
        /// Returns the location of the specified uniform variable.
        /// </summary>
        /// <param name="name">The name of the uniform variable.</param>
        /// <returns>The uniform location.</returns>
        /// <exception cref="std::runtime_error">If the uniform name is not bound.</exception>
        [[nodiscard]]
        GLint getUniformLocation(std::string const& name) const;

        /// <summary>
        /// Installs the GLSL program as part of the current rendering state.
        /// </summary>
        /// <exception cref="std::runtime_error">If the program could not be installed.</exception>
        void useProgram(void) const;

    private:
        GLuint const id{ 0 };
        std::string linkLog{};
    };

}
