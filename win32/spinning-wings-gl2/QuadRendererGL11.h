#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include "WingRenderer.h"

namespace silnith::wings::gl
{

	/// <summary>
	/// The quad renderer that uses the OpenGL 1.1 rendering path.
	/// </summary>
	/// <remarks>
	/// <para>
	/// This uses the client array functionality.
	/// </para>
	/// </remarks>
	class QuadRendererGL11 : public WingRenderer
	{
	public:
		QuadRendererGL11(void);
		QuadRendererGL11(QuadRendererGL11 const&) = delete;
		QuadRendererGL11& operator=(QuadRendererGL11 const&) = delete;
		QuadRendererGL11(QuadRendererGL11&&) noexcept = delete;
		QuadRendererGL11& operator=(QuadRendererGL11&&) noexcept = delete;
		virtual ~QuadRendererGL11(void) noexcept override;

	public:
		virtual void DrawQuad(void) const override;

	private:
		/// <summary>
		/// The number of vertices in a wing.
		/// </summary>
		static GLsizei constexpr numVertices{ 4 };

		/// <summary>
		/// The number of coordinates per vertex.
		/// </summary>
		static GLint constexpr numCoordinatesPerVertex{ 2 };

		/// <summary>
		/// The (x, y) vertex data for a single wing.
		/// </summary>
		std::array<GLfloat const, numCoordinatesPerVertex * numVertices> const wingVertices;

		/// <summary>
		/// The number of indices in a wing.
		/// </summary>
		static GLsizei constexpr numIndices{ 4 };

		/// <summary>
		/// The indices into <see cref="wingVertices"/> for rendering a single wing.
		/// </summary>
		std::array<GLuint const, numIndices> const wingIndices;
	};

}
