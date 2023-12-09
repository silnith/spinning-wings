#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Shader.h"

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
        Program(void) noexcept = default;
        ~Program(void) noexcept;
        Program(Program const&) noexcept = delete;
        Program& operator=(Program const&) noexcept = delete;
        Program(Program&&) noexcept;
        Program& operator=(Program&&) noexcept;
    public:
        Program(Shader const&, Shader const&);

        GLuint getProgram(void) const noexcept;

        void useProgram(void) const noexcept;

    private:
        GLuint id{ 0 };
        std::string linkLog{};
    };

}
