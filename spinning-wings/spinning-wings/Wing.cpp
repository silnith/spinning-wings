#include "Wing.h"


Wing::Wing(void) : Wing{ 10, 0, 15, 0.5,
	0, 0, 0,
	Color::BLACK, Color::WHITE }
{}

Wing::Wing(float _radius, float _angle, float _angleDelta, float _deltaZ,
	float _roll, float _pitch, float _yaw,
	Color const& _color, Color const& _edgeColor)
	: radius{ _radius }, angle{ _angle }, angleDelta{ _angleDelta }, deltaZ{ _deltaZ },
	roll{ _roll }, pitch{ _pitch }, yaw{ _yaw },
	color{ _color }, edgeColor{ _edgeColor }
{}

float Wing::getRadius(void) const
{
	return radius;
}

float Wing::getAngle(void) const
{
	return angle;
}

float Wing::getDeltaAngle(void) const
{
	return angleDelta;
}

float Wing::getZDelta(void) const
{
	return deltaZ;
}

float Wing::getRoll(void) const
{
	return roll;
}

float Wing::getPitch(void) const
{
	return pitch;
}

float Wing::getYaw(void) const
{
	return yaw;
}

Color const& Wing::getColor(void) const
{
	return color;
}

Color const& Wing::getEdgeColor(void) const
{
	return edgeColor;
}
