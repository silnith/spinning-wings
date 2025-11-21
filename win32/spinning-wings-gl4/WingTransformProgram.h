#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "Program.h"

#include "WingGeometry.h"
#include "WingTransformFeedback.h"
#include "VertexShader.h"

namespace silnith::wings::gl4
{

    class WingTransformProgram : public Program
    {
    public:
        WingTransformProgram(void) = delete;

        WingTransformProgram(std::shared_ptr<WingGeometry const> wingGeometry,
            std::shared_ptr<VertexShader const> rotateMatrixShader,
            std::shared_ptr<VertexShader const> translateMatrixShader);

#pragma region Rule of Five

        WingTransformProgram(WingTransformProgram const&) = delete;
        WingTransformProgram& operator=(WingTransformProgram const&) = delete;
        WingTransformProgram(WingTransformProgram&&) noexcept = delete;
        WingTransformProgram& operator=(WingTransformProgram&&) noexcept = delete;
        virtual ~WingTransformProgram(void) noexcept override;

#pragma endregion

    public:
        void TransformWing(GLfloat radius, GLfloat angle,
            GLfloat roll, GLfloat pitch, GLfloat yaw,
            GLfloat red, GLfloat green, GLfloat blue,
            WingTransformFeedback const& wingTransformFeedbackObject) const;

    private:
        std::shared_ptr<WingGeometry const> wingGeometry{ nullptr };

        /// <summary>
        /// The vertex array used for transform feedback.
        /// This maintains the state of the enabled vertex attributes.
        /// </summary>
        GLuint vertexArray{ 0 };

        GLint const radiusAngleUniformLocation{ 0 };
        GLint const rollPitchYawUniformLocation{ 0 };
        GLint const colorUniformLocation{ 0 };
        GLint const edgeColorUniformLocation{ 0 };

        GLuint const vertexAttributeLocation{ 0 };
    };

}
