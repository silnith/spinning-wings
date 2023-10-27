#include "Wing.h"


Wing::Wing(float radius, float angle, float deltaAngle, float deltaZ,
	float roll, float pitch, float yaw,
	Color const& color, Color const& edgeColor) noexcept
	: radius{ radius }, angle{ angle }, deltaAngle{ deltaAngle }, deltaZ{ deltaZ },
	roll{ roll }, pitch{ pitch }, yaw{ yaw },
	color{ color }, edgeColor{ edgeColor }
{}

float Wing::getRadius(void) const noexcept
{
	return radius;
}

float Wing::getAngle(void) const noexcept
{
	return angle;
}

float Wing::getDeltaAngle(void) const noexcept
{
	return deltaAngle;
}

float Wing::getDeltaZ(void) const noexcept
{
	return deltaZ;
}

float Wing::getRoll(void) const noexcept
{
	return roll;
}

float Wing::getPitch(void) const noexcept
{
	return pitch;
}

float Wing::getYaw(void) const noexcept
{
	return yaw;
}

Color const& Wing::getColor(void) const noexcept
{
	return color;
}

Color const& Wing::getEdgeColor(void) const noexcept
{
	return edgeColor;
}
