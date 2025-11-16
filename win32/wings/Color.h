#pragma once

#include <concepts>

namespace silnith::wings
{

	/// <summary>
	/// A simple RGB color value.
	/// </summary>
	template<std::floating_point T>
	class Color
	{
	public:
		Color(void) = default;

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
		explicit Color(T r, T g, T b) noexcept
			: red{ r }, green{ g }, blue{ b }
		{}

#pragma region Rule of Five
	public:
		Color(Color const&) = default;
		Color& operator=(Color const&) = default;
		Color(Color&&) noexcept = default;
		Color& operator=(Color&&) noexcept = default;
		virtual ~Color(void) noexcept = default;

#pragma endregion

	public:
		/// <summary>
		/// Returns the red component.
		/// </summary>
		/// <returns>the red component</returns>
		[[nodiscard]]
		inline T getRed(void) const noexcept
		{
			return red;
		}

		/// <summary>
		/// Returns the green component.
		/// </summary>
		/// <returns>the green component</returns>
		[[nodiscard]]
		inline T getGreen(void) const noexcept
		{
			return green;
		}

		/// <summary>
		/// Returns the blue component.
		/// </summary>
		/// <returns>the blue component</returns>
		[[nodiscard]]
		inline T getBlue(void) const noexcept
		{
			return blue;
		}

		/// <summary>
		/// A constant for the color black.
		/// The color components are all <c>0</c>.
		/// </summary>
		static Color<T> const BLACK;

		/// <summary>
		/// A constant for the color white.
		/// The color components are all <c>1</c>.
		/// </summary>
		static Color<T> const WHITE;

	private:
		T const red{ 0 };
		T const green{ 0 };
		T const blue{ 0 };
	};

	template<std::floating_point T>
	Color<T> const Color<T>::BLACK{ 0, 0, 0 };

	template<std::floating_point T>
	Color<T> const Color<T>::WHITE{ 1, 1, 1 };

}
