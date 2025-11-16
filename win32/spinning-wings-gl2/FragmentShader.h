#pragma once

#include <initializer_list>
#include <string>

#include "Shader.h"

namespace silnith::wings::gl2
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
#pragma region Rule of Five

    public:
        FragmentShader(FragmentShader const&) = delete;
        FragmentShader& operator=(FragmentShader const&) = delete;
        FragmentShader(FragmentShader&&) noexcept = delete;
        FragmentShader& operator=(FragmentShader&&) noexcept = delete;
        virtual ~FragmentShader(void) noexcept override = default;

#pragma endregion

    public:
        /// <summary>
        /// Default constructor is deleted.  A shader is not valid without source code.
        /// </summary>
        FragmentShader(void) = delete;

        /// <summary>
        /// Creates and compiles a fragment shader from the given GLSL sources.
        /// The source strings are concatenated.
        /// </summary>
        /// <param name="sources">The source strings to concatenate.</param>
        /// <exception cref="std::runtime_error">If an error occurs creating the shader object in the OpenGL state machine.</exception>
        explicit FragmentShader(std::initializer_list<std::string> sources);
    };

}
