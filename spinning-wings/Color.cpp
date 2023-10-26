#include "Color.h"

Color const Color::BLACK{ 0, 0, 0 };
Color const Color::WHITE{ 1, 1, 1 };

Color::Color(float red, float green, float blue) noexcept
	: red{ red }, green{ green }, blue{ blue }
{}

float Color::getRed(void) const noexcept
{
	return red;
}

float Color::getGreen(void) const noexcept
{
	return green;
}

float Color::getBlue(void) const noexcept
{
	return blue;
}
