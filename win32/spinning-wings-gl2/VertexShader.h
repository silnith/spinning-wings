#pragma once

#include "Shader.h"

#include <string>
#include <vector>

namespace silnith::wings::gl2
{
    /// <summary>
    /// Manages a single GLSL vertex shader.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
    /// </para>
    /// </remarks>
    class VertexShader : public Shader
    {
    public:
        VertexShader(void) noexcept = delete;
        VertexShader(VertexShader const&) noexcept = delete;
        VertexShader& operator=(VertexShader const&) noexcept = delete;
        VertexShader(VertexShader&&) noexcept = delete;
        VertexShader& operator=(VertexShader&&) noexcept = delete;
        ~VertexShader(void) noexcept = default;
    public:
        VertexShader(std::vector<std::string> const&);

    private:
    };

}
