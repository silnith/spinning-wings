#include "Wing.h"


Wing::Wing(float _radius, float _angle, float _angleDelta, float _deltaZ,
	float _roll, float _pitch, float _yaw,
	Color const& _color, Color const& _edgeColor) noexcept
	: radius{ _radius }, angle{ _angle }, angleDelta{ _angleDelta }, deltaZ{ _deltaZ },
	roll{ _roll }, pitch{ _pitch }, yaw{ _yaw },
	color{ _color }, edgeColor{ _edgeColor }
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
	return angleDelta;
}

float Wing::getZDelta(void) const noexcept
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
