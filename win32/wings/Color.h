#pragma once

class Color
{
public:
	Color(void) noexcept = default;
	~Color(void) noexcept = default;
	Color(Color const&) noexcept = default;
	Color& operator=(Color const&) noexcept = default;
	Color(Color&&) noexcept = default;
	Color& operator=(Color&&) noexcept = default;
public:
	/// <summary>
	/// Initializes a new color structure.
	/// </summary>
	/// <remarks>
	/// <para>
	/// Values for color components should be in the range <c>[0.0, 1.0]</c>.
	/// </para>
	/// </remarks>
	/// <param name="r">the red component, in the range <c>[0, 1]</c></param>
	/// <param name="g">the green component, in the range <c>[0, 1]</c></param>
	/// <param name="b">the blue component, in the range <c>[0, 1]</c></param>
	Color(float r, float g, float b) noexcept;

	/// <summary>
	/// Returns the red component.
	/// </summary>
	/// <returns>the red component</returns>
	float getRed(void) const noexcept;
	/// <summary>
	/// Returns the green component.
	/// </summary>
	/// <returns>the green component</returns>
	float getGreen(void) const noexcept;
	/// <summary>
	/// Returns the blue component.
	/// </summary>
	/// <returns>the blue component</returns>
	float getBlue(void) const noexcept;

	/// <summary>
	/// A constant for the color black.
	/// The color components are all <c>0</c>.
	/// </summary>
	static Color const BLACK;
	/// <summary>
	/// A constant for the color white.
	/// The color components are all <c>1</c>.
	/// </summary>
	static Color const WHITE;

private:
	float const red{ 0 };
	float const green{ 0 };
	float const blue{ 0 };
};
