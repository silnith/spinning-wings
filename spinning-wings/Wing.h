#pragma once

#include "Color.h"

class Wing
{
public:
	Wing(void);

	Wing(float _radius, float _angle, float _angleDelta, float _deltaZ,
		float _roll, float _pitch, float _yaw,
		Color const& _color,
		Color const& _edgeColor);

	float getRadius(void) const;
	float getAngle(void) const;
	float getDeltaAngle(void) const;
	float getZDelta(void) const;
	float getRoll(void) const;
	float getPitch(void) const;
	float getYaw(void) const;
	Color const& getColor(void) const;
	Color const& getEdgeColor(void) const;

private:
	float const radius;
	float const angle;
	float const angleDelta;
	float const deltaZ;
	float const roll;
	float const pitch;
	float const yaw;
	Color const color;
	Color const edgeColor;
};
