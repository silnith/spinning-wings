#pragma once

#include "Shader.h"

#include <initializer_list>
#include <string>

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
        virtual ~VertexShader(void) noexcept = default;
    public:
        /// <summary>
        /// Creates and compiles a vertex shader from the given GLSL sources.
        /// The source strings are concatenated with newlines appended to each.
        /// </summary>
        /// <param name="sources">The source strings to concatenate.</param>
        explicit VertexShader(std::initializer_list<std::string> const& sources);

    private:
    };

}
