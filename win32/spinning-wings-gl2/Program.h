#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "FragmentShader.h"
#include "VertexShader.h"

#include <string>

namespace silnith::wings::gl2
{
    /// <summary>
    /// Manages a GLSL program.  This is composed of shaders.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
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
        /// Creates and links a program from a given vertex and fragment shader.
        /// </summary>
        /// <param name="vertexShader">The vertex shader.</param>
        /// <param name="fragmentShader">The fragment shader.</param>
        explicit Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader);

        /// <summary>
        /// Returns the OpenGL name for the program.
        /// </summary>
        /// <returns>The OpenGL identifier for the program.</returns>
        [[nodiscard]]
        GLuint getProgram(void) const noexcept;

        [[nodiscard]]
        GLuint getAttributeLocation(std::string const&) const;

        void useProgram(void) const;

    private:
        GLuint const id{ 0 };
        std::string linkLog{};
    };

}
