#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
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
    public:
        ModelViewProjectionUniformBuffer(void) = delete;
        ModelViewProjectionUniformBuffer(ModelViewProjectionUniformBuffer const&) = delete;
        ModelViewProjectionUniformBuffer& operator=(ModelViewProjectionUniformBuffer const&) = delete;
        ModelViewProjectionUniformBuffer(ModelViewProjectionUniformBuffer&&) noexcept = delete;
        ModelViewProjectionUniformBuffer& operator=(ModelViewProjectionUniformBuffer&&) noexcept = delete;
		virtual ~ModelViewProjectionUniformBuffer(void) noexcept override = default;

    public:
		/// <summary>
		/// Constructs a new uniform buffer that stores the model, view, and
		/// projection matrices expected by the uniform block of the
		/// <see cref="WingRenderProgram"/>.
		/// </summary>
		/// <param name="dataSize">The buffer size needed for the entire uniform block.</param>
		/// <param name="modelOffset">The offset within the buffer of the <c>model</c> matrix.</param>
		/// <param name="viewOffset">The offset within the buffer of the <c>view</c> matrix.</param>
		/// <param name="projectionOffset">The offset within the buffer of the <c>projection</c> matrix.</param>
		explicit ModelViewProjectionUniformBuffer(GLsizei dataSize,
			GLintptr modelOffset,
			GLintptr viewOffset,
			GLintptr projectionOffset);

		/// <summary>
		/// Sets up the orthographic projection that transforms modelview coordinates
		/// into normalized device coordinates.
		/// This takes into account the aspect ratio of the viewport.
		/// </summary>
		/// <param name="width">The viewport width.</param>
		/// <param name="height">The viewport height.</param>
        void SetProjectionMatrix(GLfloat width, GLfloat height);

    private:
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
