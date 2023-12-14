#pragma once

#include "Shader.h"

#include <string>
#include <vector>

namespace silnith::wings::gl3
{
    /// <summary>
    /// Manages a single GLSL fragment shader.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
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
        ~FragmentShader(void) noexcept = default;
    public:
        FragmentShader(std::vector<std::string> const&);

    private:
    };

}
