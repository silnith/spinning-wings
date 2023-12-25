#pragma once

#include "Shader.h"

#include <initializer_list>
#include <string>

namespace silnith::wings::gl4
{
    /// <summary>
    /// Manages a single GLSL vertex shader.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 4.1 or greater.
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
        virtual ~VertexShader(void) noexcept = default;
    public:
        explicit VertexShader(std::initializer_list<std::string> const&);

    private:
    };

}
