#pragma once

#include <Windows.h>
#include <gl/GL.h>

#include "QuadRenderer.h"

namespace silnith::wings::gl
{

	/// <summary>
	/// The quad renderer that uses the OpenGL 1.0 rendering path.
	/// </summary>
	class QuadRendererGL10 : public QuadRenderer
	{
	public:
		QuadRendererGL10(void);
		QuadRendererGL10(QuadRendererGL10 const&) = delete;
		QuadRendererGL10& operator=(QuadRendererGL10 const&) = delete;
		QuadRendererGL10(QuadRendererGL10&&) noexcept = delete;
		QuadRendererGL10& operator=(QuadRendererGL10&&) noexcept = delete;
		virtual ~QuadRendererGL10(void) noexcept override;

	public:
		virtual void DrawQuad(void) const override;

	private:
		/// <summary>
		/// The GL display list for rendering a single quad.
		/// </summary>
		GLuint const quadDisplayList{ 0 };
	};

}
