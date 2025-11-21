#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <array>
#include <memory>
#include <string>

#include "Buffer.h"

namespace silnith::wings::gl4
{

    /// <summary>
    /// A class to reimplement the ModelView and Projection matrices that were
    /// removed from the OpenGL 3.2 Core profile.  The GLSL programs now take
    /// in uniforms instead.  The uniforms are provided in a buffer.
    /// </summary>
    /// <example>
    /// The GLSL declaration for this is:
    /// <code>
    /// uniform ModelViewProjection{
    ///     mat4 model;
    ///     mat4 view;
    ///     mat4 projection;
    /// };
    /// </code>
    /// </example>
    /// <remarks>
    /// <para>
    /// This implementation writes matrices to the buffer as column-major,
    /// which is historically what OpenGL expects.
    /// </para>
    /// </remarks>
    class ModelViewProjectionUniformBuffer : public Buffer
    {
#pragma region Static Members

    public:
        /// <summary>
        /// The GLSL declaration of the <c>ModelViewProjection</c> named uniform
        /// block.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Every shader that wants to use this uniform buffer should include
        /// this declaration.  The default memory layout is <c>shared</c>,
        /// which means every program that includes this can map to the same
        /// uniform buffer binding point and read the uniform variables from
        /// the same buffer.
        /// </para>
        /// </remarks>
        static std::string const uniformBlockDeclaration;

        /// <summary>
        /// Create and initialize a new uniform buffer by querying the memory
        /// layout of the provide program object.  The program must include
        /// <see cref="uniformBlockDeclaration"/> in its shader source code,
        /// and must reference it so that the uniform is considered active.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This implicitly calls <see cref="UseForProgram"/> for the program
        /// in addition to creating the uniform buffer and binding it to the
        /// shared binding point.
        /// </para>
        /// </remarks>
        /// <param name="programName">The OpenGL name for the program object.</param>
        /// <returns>A newly-allocated uniform buffer.</returns>
        /// <exception cref="std::runtime_error">If there was any problem creating the buffer.</exception>
        static std::shared_ptr<ModelViewProjectionUniformBuffer const> MakeBuffer(GLuint programName, GLuint bindingPoint);

#pragma endregion

    public:
        /// <summary>
        /// Default constructor is deleted.  A uniform buffer is not valid
        /// without knowing the offsets of its contents.
        /// </summary>
        ModelViewProjectionUniformBuffer(void) = delete;

        /// <summary>
        /// Constructs a new uniform buffer that stores the model, view, and
        /// projection matrices expected by the uniform block of the
        /// <see cref="WingRenderProgram"/>.
        /// </summary>
        /// <param name="bindingPoint">The index of the global binding point that this buffer will use.</param>
        /// <param name="dataSize">The buffer size needed for the entire uniform block.</param>
        /// <param name="modelOffset">The offset within the buffer of the <c>model</c> matrix.</param>
        /// <param name="viewOffset">The offset within the buffer of the <c>view</c> matrix.</param>
        /// <param name="projectionOffset">The offset within the buffer of the <c>projection</c> matrix.</param>
        explicit ModelViewProjectionUniformBuffer(
            GLuint bindingPoint,
            GLsizei dataSize,
            GLintptr modelOffset,
            GLintptr viewOffset,
            GLintptr projectionOffset);

#pragma region Rule of Five

    public:
        ModelViewProjectionUniformBuffer(ModelViewProjectionUniformBuffer const&) = delete;
        ModelViewProjectionUniformBuffer& operator=(ModelViewProjectionUniformBuffer const&) = delete;
        ModelViewProjectionUniformBuffer(ModelViewProjectionUniformBuffer&&) noexcept = delete;
        ModelViewProjectionUniformBuffer& operator=(ModelViewProjectionUniformBuffer&&) noexcept = delete;
        virtual ~ModelViewProjectionUniformBuffer(void) noexcept override;

#pragma endregion

    public:
        /// <summary>
        /// Returns the index into the array of uniform buffer binding points
        /// that this buffer is bound to.
        /// </summary>
        /// <returns>The global binding point for this buffer.</returns>
        GLuint getBindingPoint(void) const noexcept;

        /// <summary>
        /// Maps the uniform block in the named program to use this buffer for
        /// its contents.  The program must include <see cref="uniformBlockDeclaration"/>
        /// in its source code.
        /// </summary>
        /// <param name="programName">The OpenGL name for the program object.</param>
        /// <exception cref="std::runtime_error">If the program did not include <see cref="uniformBlockDeclaration"/>.</exception>
        void UseForProgram(GLuint programName) const;

        /// <summary>
        /// Replaces the current model matrix in the buffer with the new value.
        /// The matrix must be stored in column-major order.
        /// </summary>
        /// <param name="modelMatrix">The new value for the model matrix.</param>
        void SetModelMatrix(std::array<GLfloat, 4 * 4> const& modelMatrix) const;

        /// <summary>
        /// Replaces the current view matrix in the buffer with the new value.
        /// The matrix must be stored in column-major order.
        /// </summary>
        /// <param name="modelMatrix">The new value for the view matrix.</param>
        void SetViewMatrix(std::array<GLfloat, 4 * 4> const& viewMatrix) const;

        /// <summary>
        /// Replaces the current projection matrix in the buffer with the new value.
        /// The matrix must be stored in column-major order.
        /// </summary>
        /// <param name="modelMatrix">The new value for the projection matrix.</param>
        void SetProjectionMatrix(std::array<GLfloat, 4 * 4> const& projectionMatrix) const;

    private:
        /// <summary>
        /// Writes the provided matrix into the buffer at the given offset.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This is a convenience function for the public methods above,
        /// because the code to write is the same for all of them.
        /// </para>
        /// </remarks>
        /// <param name="offset">The offset inside the buffer where the matrix will be written.</param>
        /// <param name="matrix">The matrix to write.</param>
        void SetMatrix(GLintptr offset, std::array<GLfloat, 4 * 4> const& matrix) const;

    private:
        /// <summary>
        /// The binding point that this buffer will be bound to.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Multiple GLSL programs can use a single uniform buffer.
        /// Since the index of the uniform block can vary between programs,
        /// there is a global array of uniform buffer binding points, and each
        /// program can specify the mapping from its individual uniform block
        /// index to the global uniform buffer binding point.
        /// </para>
        /// <para>
        /// This value is the global binding point that this buffer is bound to.
        /// </para>
        /// </remarks>
        GLuint const bindingPoint{ 0 };

        /// <summary>
        /// The offset into the uniform buffer where the model matrix is.
        /// </summary>
        GLintptr const modelOffset{ 0 };

        /// <summary>
        /// The offset into the uniform buffer where the view matrix is.
        /// </summary>
        GLintptr const viewOffset{ 0 };

        /// <summary>
        /// The offset into the uniform buffer where the projection matrix is.
        /// </summary>
        GLintptr const projectionOffset{ 0 };
    };

}
