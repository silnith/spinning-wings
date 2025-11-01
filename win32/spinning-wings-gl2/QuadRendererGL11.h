#pragma once

#include "QuadRenderer.h"

namespace silnith::wings::gl
{

	/// <summary>
	/// The quad renderer that uses the OpenGL 1.1 rendering path.
	/// </summary>
	class QuadRendererGL11 : public QuadRenderer
	{
	public:
		QuadRendererGL11(void);
		QuadRendererGL11(QuadRendererGL11 const&) = delete;
		QuadRendererGL11& operator=(QuadRendererGL11 const&) = delete;
		QuadRendererGL11(QuadRendererGL11&&) noexcept = delete;
		QuadRendererGL11& operator=(QuadRendererGL11&&) noexcept = delete;
		virtual ~QuadRendererGL11(void) noexcept override;

	public:
		virtual void DrawQuad(void) const;
	};

}
