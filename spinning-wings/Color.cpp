#include "Color.h"

Color const Color::BLACK{ 0, 0, 0 };
Color const Color::WHITE{ 1, 1, 1 };

Color::Color(float r, float g, float b) noexcept : red{ r }, green{ g }, blue{ b }
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
