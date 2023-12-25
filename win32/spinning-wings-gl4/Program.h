#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "FragmentShader.h"
#include "VertexShader.h"

#include <initializer_list>
#include <string>

namespace silnith::wings::gl4
{
    /// <summary>
    /// Manages a GLSL program.  This is composed of shaders.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 4.1 or greater.
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
        /// <param name="vertexShader">The vertex shader.</param>
        /// <param name="capturedVaryings">The varying variables to capture.</param>
        explicit Program(VertexShader const& vertexShader, std::initializer_list<std::string> const& capturedVaryings);

        /// <summary>
        /// Creates a GLSL program for rendering.
        /// </summary>
        /// <param name="vertexShader">The vertex shader.</param>
        /// <param name="fragmentShader">The fragment shader.</param>
        /// <param name="fragmentData">The fragment shader output variable to be written into the output buffer.</param>
        explicit Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader, std::string const& fragmentData);

        GLuint getProgram(void) const noexcept;

        GLuint getAttributeLocation(std::string const&) const;

        GLint getUniformLocation(std::string const&) const;

        void useProgram(void) const noexcept;

    private:
        GLuint id{ 0 };
        std::string linkLog{};
    };

}
