#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <memory>
#include <string>

#include "Shader.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// Manages a single GLSL vertex shader.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 3.2 or greater.
    /// </para>
    /// </remarks>
    class VertexShader : public Shader
    {
#pragma region Static Members

    public:
        /// <summary>
        /// A convenience factory method for creating a vertex shader that
        /// provides the functionality that used to be provided by
        /// <c>glRotate</c>.  Any vertex shader that wants to link against this
        /// should include the function declaration contained in
        /// <see cref="Shader::rotateMatrixFunctionDeclaration"/>.
        /// </summary>
        /// <returns>A vertex shader that can be linked into a program.</returns>
        static std::shared_ptr<VertexShader const> MakeRotateMatrixShader(void);

        /// <summary>
        /// A convenience factory method for creating a vertex shader that
        /// provides the functionality that used to be provided by
        /// <c>glTranslate</c>.  Any vertex shader that wants to link against this
        /// should include the function declaration contained in
        /// <see cref="Shader::translateMatrixFunctionDeclaration"/>.
        /// </summary>
        /// <returns>A vertex shader that can be linked into a program.</returns>
        static std::shared_ptr<VertexShader const> MakeTranslateMatrixShader(void);

        /// <summary>
        /// A convenience factory method for creating a vertex shader that
        /// provides the functionality that used to be provided by
        /// <c>glScale</c>.  Any vertex shader that wants to link against this
        /// should include the function declaration contained in
        /// <see cref="Shader::scaleMatrixFunctionDeclaration"/>.
        /// </summary>
        /// <returns>A vertex shader that can be linked into a program.</returns>
        static std::shared_ptr<VertexShader const> MakeScaleMatrixShader(void);

#pragma endregion

    public:
        /// <summary>
        /// Default constructor is deleted.  A shader is not valid without source code.
        /// </summary>
        VertexShader(void) = delete;

        /// <summary>
        /// Creates and compiles a vertex shader from the given GLSL sources.
        /// The source strings are concatenated.
        /// </summary>
        /// <param name="sources">The source strings to concatenate.</param>
        /// <exception cref="std::runtime_error">If an error occurs creating the shader object in the OpenGL state machine.</exception>
        explicit VertexShader(std::initializer_list<std::string> sources);

#pragma region Rule of Five

    public:
        VertexShader(VertexShader const&) = delete;
        VertexShader& operator=(VertexShader const&) = delete;
        VertexShader(VertexShader&&) noexcept = delete;
        VertexShader& operator=(VertexShader&&) noexcept = delete;
        virtual ~VertexShader(void) noexcept override = default;

#pragma endregion

    };

}
