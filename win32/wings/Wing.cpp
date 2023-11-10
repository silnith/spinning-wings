#include "Wing.h"

silnith::Wing::Wing(unsigned int displayList) noexcept : displayList{ displayList }
{}

silnith::Wing::Wing(unsigned int displayList,
	float radius, float angle, float deltaAngle, float deltaZ,
	float roll, float pitch, float yaw,
	silnith::Color const& color, silnith::Color const& edgeColor) noexcept
	: displayList{ displayList },
	radius{ radius }, angle{ angle }, deltaAngle{ deltaAngle }, deltaZ{ deltaZ },
	roll{ roll }, pitch{ pitch }, yaw{ yaw },
	color{ color }, edgeColor{ edgeColor }
{}

unsigned int silnith::Wing::getGLDisplayList(void) const noexcept
{
	return displayList;
}

float silnith::Wing::getRadius(void) const noexcept
{
	return radius;
}

float silnith::Wing::getAngle(void) const noexcept
{
	return angle;
}

float silnith::Wing::getDeltaAngle(void) const noexcept
{
	return deltaAngle;
}

float silnith::Wing::getDeltaZ(void) const noexcept
{
	return deltaZ;
}

float silnith::Wing::getRoll(void) const noexcept
{
	return roll;
}

float silnith::Wing::getPitch(void) const noexcept
{
	return pitch;
}

float silnith::Wing::getYaw(void) const noexcept
{
	return yaw;
}

silnith::Color const& silnith::Wing::getColor(void) const noexcept
{
	return color;
}

silnith::Color const& silnith::Wing::getEdgeColor(void) const noexcept
{
	return edgeColor;
}
