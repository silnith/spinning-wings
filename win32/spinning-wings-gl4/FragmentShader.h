#pragma once

#include "Shader.h"

#include <initializer_list>
#include <string>

namespace silnith::wings::gl4
{
    /// <summary>
    /// Manages a single GLSL fragment shader.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 4.1 or greater.
    /// </para>
    /// </remarks>
    class FragmentShader : public Shader
    {
    public:
        FragmentShader(void) noexcept = delete;
        FragmentShader(FragmentShader const&) noexcept = delete;
        FragmentShader& operator=(FragmentShader const&) noexcept = delete;
        FragmentShader(FragmentShader&&) noexcept = delete;
        FragmentShader& operator=(FragmentShader&&) noexcept = delete;
        virtual ~FragmentShader(void) noexcept = default;
    public:
        explicit FragmentShader(std::initializer_list<std::string> const&);

    private:
    };

}
