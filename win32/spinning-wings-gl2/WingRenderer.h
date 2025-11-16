#pragma once

namespace silnith::wings::gl
{

	/// <summary>
	/// Interface for utility classes that draw a single wing.
	/// </summary>
	/// <remarks>
	/// <para>
	/// There will be implementations for OpenGL 1.0, 1.1, and 1.5.
	/// </para>
	/// </remarks>
	class WingRenderer
	{
	public:
		explicit WingRenderer(void) = default;

#pragma region Rule of Five

	public:
		WingRenderer(WingRenderer const&) = delete;
		WingRenderer& operator=(WingRenderer const&) = delete;
		WingRenderer(WingRenderer&&) noexcept = delete;
		WingRenderer& operator=(WingRenderer&&) noexcept = delete;
		virtual ~WingRenderer(void) noexcept = default;

#pragma endregion

	public:
		/// <summary>
		/// Draws a single wing.  This is suitable for compiling
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
		virtual void DrawWing(void) const = 0;
	};

}
