#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "WingRenderer.h"

namespace silnith::wings::gl
{

	/// <summary>
	/// The quad renderer that uses the OpenGL 1.0 rendering path.
	/// </summary>
	/// <remarks>
	/// <para>
	/// This simply provides the vertices with direct calls and captures them
	/// in a display list.
	/// </para>
	/// </remarks>
	class WingRendererGL10 : public WingRenderer
	{
	public:
		WingRendererGL10(void);
		WingRendererGL10(WingRendererGL10 const&) = delete;
		WingRendererGL10& operator=(WingRendererGL10 const&) = delete;
		WingRendererGL10(WingRendererGL10&&) noexcept = delete;
		WingRendererGL10& operator=(WingRendererGL10&&) noexcept = delete;
		virtual ~WingRendererGL10(void) noexcept override;

	public:
		virtual void DrawWing(void) const override;

	private:
		/// <summary>
		/// The GL display list for rendering a single quad.
		/// </summary>
		GLuint const quadDisplayList{ 0 };
	};

}
