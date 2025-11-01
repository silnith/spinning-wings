#pragma once

namespace silnith::wings::gl
{
	/// <summary>
	/// Interface for utility classes that draw a single quad.
	/// </summary>
	/// <remarks>
	/// <para>
	/// There will be implementations for OpenGL 1.0, 1.1, and 1.5.
	/// </para>
	/// </remarks>
	class QuadRenderer
	{
	public:
		QuadRenderer(void) = default;
		QuadRenderer(QuadRenderer const&) = delete;
		QuadRenderer& operator=(QuadRenderer const&) = delete;
		QuadRenderer(QuadRenderer&&) noexcept = delete;
		QuadRenderer& operator=(QuadRenderer&&) noexcept = delete;
		virtual ~QuadRenderer(void) noexcept = default;

	public:
		/// <summary>
		/// Draws a single quad to represent a wing.  This is suitable for compiling
		/// into a display list, or for immediate-mode rendering.
		/// </summary>
		/// <remarks>
		/// <para>
		/// This only specifies the geometry in the proper order.
		/// It does not specify colors or rendering mode.
		/// The quad will cover an (x, y) square matching
		/// normalized device coordinates, namely <c>[-1, 1]</c>.
		/// </para>
		/// </remarks>
		virtual void DrawQuad(void) const = 0;
	};
}
