#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "Program.h"

#include "WingGeometry.h"
#include "WingTransformFeedback.h"
#include "RotateVertexShader.h"
#include "TranslateVertexShader.h"
#include "VertexShader.h"

namespace silnith::wings::gl4
{

    class WingTransformProgram : public Program
    {
    public:
        /// <summary>
        /// Default constructor is deleted.  The transformation program
        /// requires the wing geometry and utility shaders that are shared
        /// between multiple programs.
        /// </summary>
        WingTransformProgram(void) = delete;

        WingTransformProgram(std::shared_ptr<WingGeometry const> wingGeometry,
            std::shared_ptr<RotateVertexShader const> rotateMatrixShader,
            std::shared_ptr<TranslateVertexShader const> translateMatrixShader);

#pragma region Rule of Five

        WingTransformProgram(WingTransformProgram const&) = delete;
        WingTransformProgram& operator=(WingTransformProgram const&) = delete;
        WingTransformProgram(WingTransformProgram&&) noexcept = delete;
        WingTransformProgram& operator=(WingTransformProgram&&) noexcept = delete;
        virtual ~WingTransformProgram(void) noexcept override;

#pragma endregion

    public:
        /// <summary>
        /// Creates and returns a transform feedback object initialized with
        /// destination buffers of appropriate size for receiving the
        /// transformed wing geometry.
        /// </summary>
        /// <returns>The transform feedback object.</returns>
        std::shared_ptr<WingTransformFeedback const> CreateTransformFeedback(void) const;

        /// <summary>
        /// Generates the transformed vertex data for a new wing.
        /// This applies the rotations and translations to put the wing in the
        /// correct place, and places the vertex coordinates and colors for the
        /// wing into the buffers specified by the transform feedback object.
        /// </summary>
        /// <param name="radius">The radius of the wing around the central axis.</param>
        /// <param name="angle">The angle of the wing around the central axis.</param>
        /// <param name="roll">The roll of the wing.</param>
        /// <param name="pitch">The pitch of the wing.</param>
        /// <param name="yaw">The yaw of the wing.</param>
        /// <param name="red">The red component of the wing color.</param>
        /// <param name="green">The green component of the wing color.</param>
        /// <param name="blue">The blue component of the wing color.</param>
        /// <param name="wingTransformFeedbackObject">The buffers that will be populated with the transformed wing.</param>
        void TransformWing(GLfloat radius, GLfloat angle,
            GLfloat roll, GLfloat pitch, GLfloat yaw,
            GLfloat red, GLfloat green, GLfloat blue,
            WingTransformFeedback const& wingTransformFeedbackObject) const;

    private:
        /// <summary>
        /// The source wing geometry that this program transforms and captures.
        /// </summary>
        std::shared_ptr<WingGeometry const> wingGeometry{ nullptr };

        /// <summary>
        /// The vertex array used for transform feedback.
        /// This maintains the state of the enabled vertex attributes.
        /// </summary>
        GLuint vertexArray{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>radiusAngle</c>.
        /// </summary>
        /// <seealso cref="glUniform2f"/>
        GLint const radiusAngleUniformLocation{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>rollPitchYaw</c>.
        /// </summary>
        /// <seealso cref="glUniform3f"/>
        GLint const rollPitchYawUniformLocation{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>color</c>.
        /// </summary>
        /// <seealso cref="glUniform3f"/>
        GLint const colorUniformLocation{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>edgeColor</c>.
        /// </summary>
        /// <seealso cref="glUniform3f"/>
        GLint const edgeColorUniformLocation{ 0 };
    };

}
