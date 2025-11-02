#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "QuadRenderer.h"

namespace silnith::wings::gl
{

	/// <summary>
	/// The quad renderer that uses the OpenGL 1.5 rendering path.
	/// </summary>
	class QuadRendererGL15 : public QuadRenderer
	{
	public:
		QuadRendererGL15(void);
		QuadRendererGL15(QuadRendererGL15 const&) = delete;
		QuadRendererGL15& operator=(QuadRendererGL15 const&) = delete;
		QuadRendererGL15(QuadRendererGL15&&) noexcept = delete;
		QuadRendererGL15& operator=(QuadRendererGL15&&) noexcept = delete;
		virtual ~QuadRendererGL15(void) noexcept override;

	public:
		virtual void DrawQuad(void) const override;

	private:
		GLuint wingBufferObject{ 0 };
		GLuint wingIndicesBufferObject{ 0 };
	};

}
