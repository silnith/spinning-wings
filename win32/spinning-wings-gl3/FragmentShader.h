#pragma once

#include <initializer_list>
#include <string>

#include "Shader.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// Manages a single GLSL fragment shader.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 3.2 or greater.
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
        virtual ~FragmentShader(void) noexcept override = default;

    public:
        /// <summary>
        /// Creates and compiles a fragment shader from the given GLSL sources.
        /// The source strings are concatenated with newlines appended to each.
        /// </summary>
        /// <param name="sources">The source strings to concatenate.</param>
        /// <exception cref="std::runtime_error">If an error occurs creating the shader object in the OpenGL state machine.</exception>
        explicit FragmentShader(std::initializer_list<std::string> sources);
    };

}
