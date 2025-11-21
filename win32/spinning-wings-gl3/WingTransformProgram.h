#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "Program.h"

#include "WingGeometry.h"

#include "ArrayBuffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// The GLSL program that transforms each wing and captures the results
    /// using transform feedback.
    /// </summary>
    class WingTransformProgram : public Program
    {
    public:
        /// <summary>
        /// Default constructor is deleted.  The transform program requires the
        /// wing geometry and utility shaders that are shared between multiple
        /// programs.
        /// </summary>
        WingTransformProgram(void) = delete;

        explicit WingTransformProgram(std::shared_ptr<WingGeometry const> const& wingGeometry,
            std::shared_ptr<VertexShader const> const& rotateMatrixShader,
            std::shared_ptr<VertexShader const> const& translateMatrixShader);

#pragma region Rule of Five

    public:
        WingTransformProgram(WingTransformProgram const&) = delete;
        WingTransformProgram& operator=(WingTransformProgram const&) = delete;
        WingTransformProgram(WingTransformProgram&&) noexcept = delete;
        WingTransformProgram& operator=(WingTransformProgram&&) noexcept = delete;
        virtual ~WingTransformProgram(void) noexcept override;

#pragma endregion

    public:
        /// <summary>
        /// Allocates a buffer object to serve as the recipient of the
        /// transform feedback program.  The buffer will be of sufficient size
        /// and configured with the appropriate format to receive vertex coordinates.
        /// </summary>
        /// <returns>A pre-allocated empty buffer for receiving transformed vertex coordinates.</returns>
        std::shared_ptr<ArrayBuffer const> CreateVertexBuffer() const;

        /// <summary>
        /// Allocates a buffer object to serve as the recipient of the
        /// transform feedback program.  The buffer will be of sufficient size
        /// and configured with the appropriate format to receive color values.
        /// </summary>
        /// <returns>A pre-allocated empty buffer for receiving transformed color values.</returns>
        std::shared_ptr<ArrayBuffer const> CreateColorBuffer() const;

        /// <summary>
        /// Generates the transformed vertex data for a new wing.
        /// This applies the rotations and translations to put the wing in the
        /// correct place, and places the vertex coordinates and colors for the
        /// wing into the buffers provided.
        /// </summary>
        /// <param name="radius">The radius of the wing around the central axis.</param>
        /// <param name="angle">The angle of the wing around the central axis.</param>
        /// <param name="roll">The roll of the wing.</param>
        /// <param name="pitch">The pitch of the wing.</param>
        /// <param name="yaw">The yaw of the wing.</param>
        /// <param name="red">The red component of the wing color.</param>
        /// <param name="green">The green component of the wing color.</param>
        /// <param name="blue">The blue component of the wing color.</param>
        /// <param name="vertexBuffer">The buffer that will be populated with the wing geometry.</param>
        /// <param name="colorBuffer">The buffer that will be populated with the wing surface color.</param>
        /// <param name="edgeColorBuffer">The buffer that will be populated with the wing edge color.</param>
        void TransformWing(GLfloat radius, GLfloat angle,
            GLfloat roll, GLfloat pitch, GLfloat yaw,
            GLfloat red, GLfloat green, GLfloat blue,
            ArrayBuffer const& vertexBuffer,
            ArrayBuffer const& colorBuffer,
            ArrayBuffer const& edgeColorBuffer) const;

    private:
        /// <summary>
        /// A pointer to the wing geometry object.
        /// </summary>
        std::shared_ptr<WingGeometry const> wingGeometry{ nullptr };

        /// <summary>
        /// The vertex array object that captures the <see cref="GL_ARRAY_BUFFER"/>
        /// that contains the source vertices to transform.
        /// </summary>
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
