#include "Color.h"

silnith::Color const silnith::Color::BLACK{ 0, 0, 0 };
silnith::Color const silnith::Color::WHITE{ 1, 1, 1 };

silnith::Color::Color(float red, float green, float blue) noexcept
	: red{ red }, green{ green }, blue{ blue }
{}

float silnith::Color::getRed(void) const noexcept
{
	return red;
}

float silnith::Color::getGreen(void) const noexcept
{
	return green;
}

float silnith::Color::getBlue(void) const noexcept
{
	return blue;
}
