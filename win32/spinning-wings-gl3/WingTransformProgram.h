#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Program.h"

#include "OriginalVertexBuffer.h"

#include "TransformedVertexBuffer.h"
#include "TransformedColorBuffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// The GLSL program that transforms each wing and captures the results
    /// using transform feedback.
    /// </summary>
    class WingTransformProgram : public Program
    {
    private:
        static std::string const sourceCode;

    public:
        WingTransformProgram(void);
        WingTransformProgram(WingTransformProgram const&) = delete;
        WingTransformProgram& operator=(WingTransformProgram const&) = delete;
        WingTransformProgram(WingTransformProgram&&) noexcept = delete;
        WingTransformProgram& operator=(WingTransformProgram&&) noexcept = delete;
        virtual ~WingTransformProgram(void) noexcept override;

    public:
        void Render(GLfloat radius, GLfloat angle,
            GLfloat roll, GLfloat pitch, GLfloat yaw,
            GLfloat red, GLfloat green, GLfloat blue,
            TransformedVertexBuffer const& vertexBuffer,
            TransformedColorBuffer const& colorBuffer,
            TransformedColorBuffer const& edgeColorBuffer) const;

    private:
        /// <summary>
        /// A buffer object containing the original untransformed vertices for
        /// a wing.
        /// </summary>
        OriginalVertexBuffer const originalVertices{};

        /// <summary>
        /// The vertex array object that captures the <see cref="GL_ARRAY_BUFFER"/>
        /// that contains the source vertices to transform.
        /// </summary>
        /// <see cref="originalVertices"/>
        GLuint vertexArray{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>radiusAngle</c>.
        /// </summary>
        /// <seealso cref="glUniform2f"/>
        GLint radiusAngleUniformLocation{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>rollPitchYaw</c>.
        /// </summary>
        /// <seealso cref="glUniform3f"/>
        GLint rollPitchYawUniformLocation{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>color</c>.
        /// </summary>
        /// <seealso cref="glUniform3f"/>
        GLint colorUniformLocation{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>edgeColor</c>.
        /// </summary>
        /// <seealso cref="glUniform3f"/>
        GLint edgeColorUniformLocation{ 0 };
    };

}
