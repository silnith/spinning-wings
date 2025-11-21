#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <string>

#include "VertexShader.h"

namespace silnith::wings::gl4
{

    /// <summary>
    /// An explicit type for a vertex shader that implements the GLSL <c>translate</c>
    /// function, duplicating the functionality that used to be provided by the
    /// standard OpenGL <c>glTranslate</c> function.
    /// </summary>
    class TranslateVertexShader : public VertexShader
    {
    public:
        explicit TranslateVertexShader(void);

#pragma region Rule of Five

        TranslateVertexShader(TranslateVertexShader const&) = delete;
        TranslateVertexShader& operator=(TranslateVertexShader const&) = delete;
        TranslateVertexShader(TranslateVertexShader&&) noexcept = delete;
        TranslateVertexShader& operator=(TranslateVertexShader&&) noexcept = delete;
        virtual ~TranslateVertexShader(void) noexcept override = default;

#pragma endregion

    public:
        /// <summary>
        /// Returns the GLSL declaration for the <c>translate</c> function.
        /// This must be included in the source code of any shader that will
        /// call the function.
        /// </summary>
        /// <returns>The GLSL function declaration.</returns>
        std::string GetDeclaration(void) const;
    };

}
