#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "WingRenderer.h"

namespace silnith::wings::gl
{

	/// <summary>
	/// The quad renderer that uses the OpenGL 1.5 rendering path.
	/// </summary>
	/// <remarks>
	/// <para>
	/// This uses buffer objects to hold the vertex and element arrays.
	/// </para>
	/// </remarks>
	class QuadRendererGL15 : public WingRenderer
	{
	public:
		QuadRendererGL15(void);
		QuadRendererGL15(QuadRendererGL15 const&) = delete;
		QuadRendererGL15& operator=(QuadRendererGL15 const&) = delete;
		QuadRendererGL15(QuadRendererGL15&&) noexcept = delete;
		QuadRendererGL15& operator=(QuadRendererGL15&&) noexcept = delete;
		virtual ~QuadRendererGL15(void) noexcept override;

	public:
		virtual void DrawWing(void) const override;

	private:
		/// <summary>
		/// A buffer to hold the vertex coordinates.
		/// This is bound as the <c>GL_ARRAY_BUFFER</c>.
		/// </summary>
		GLuint wingBufferObject{ 0 };

		/// <summary>
		/// A buffer to hold the indices of the vertices.
		/// This is bound as the <c>GL_ELEMENT_ARRAY_BUFFER</c>.
		/// </summary>
		GLuint wingIndicesBufferObject{ 0 };

		/// <summary>
		/// The number of indices that make up a quad.
		/// </summary>
		static GLsizei constexpr numIndices{ 4 };
	};

}
