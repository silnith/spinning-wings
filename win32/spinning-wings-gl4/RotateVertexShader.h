#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <string>

#include "VertexShader.h"

namespace silnith::wings::gl4
{

    /// <summary>
    /// An explicit type for a vertex shader that implements the GLSL <c>rotate</c>
    /// function, duplicating the functionality that used to be provided by the
    /// standard OpenGL <c>glRotate</c> function.
    /// </summary>
    class RotateVertexShader : public VertexShader
    {
    public:
        explicit RotateVertexShader(void);

#pragma region Rule of Five

        RotateVertexShader(RotateVertexShader const&) = delete;
        RotateVertexShader& operator=(RotateVertexShader const&) = delete;
        RotateVertexShader(RotateVertexShader&&) noexcept = delete;
        RotateVertexShader& operator=(RotateVertexShader&&) noexcept = delete;
        virtual ~RotateVertexShader(void) noexcept override = default;

#pragma endregion

    public:
        /// <summary>
        /// Returns the GLSL declaration for the <c>rotate</c> function.
        /// This must be included in the source code of any shader that will
        /// call the function.
        /// </summary>
        /// <returns>The GLSL function declaration.</returns>
        std::string GetDeclaration(void) const;
    };

}
