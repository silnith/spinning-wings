#include "CurveGenerator.h"

#include <algorithm>

#include <cmath>

std::random_device CurveGenerator::randomDevice{ "default" };

CurveGenerator::CurveGenerator(float _initialValue, float _minimumValue, float _maximumValue, bool _valueWraps,
	float _maximumVelocity, float _maximumAcceleration, unsigned int _ticksPerAccelerationChange)
	: minimumValue{ _minimumValue }, maximumValue{ _maximumValue }, valueWraps{ _valueWraps },
	maximumVelocity{ _maximumVelocity }, maximumAcceleration{ _maximumAcceleration }, ticksPerAccelerationChange{ _ticksPerAccelerationChange },
	distributor{ -_maximumAcceleration, _maximumAcceleration },
	value{ _initialValue }, velocity{ 0 }, acceleration{ 0 }, ticks{ _ticksPerAccelerationChange }
{
	//ticks = std::uniform_int_distribution<unsigned int>{ 0, ticksPerAccelerationChange }(randomDevice);
}

CurveGenerator CurveGenerator::createGeneratorForAngles(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
{
	return CurveGenerator{ initialValue, 0.0f, 360.0f, true, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
}

CurveGenerator CurveGenerator::createGeneratorForColorComponents(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
{
	return CurveGenerator{ initialValue, 0.0f, 1.0f, false, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
}

float CurveGenerator::getMinimumValue(void) const
{
	return minimumValue;
}

float CurveGenerator::getMaximumValue(void) const
{
	return maximumValue;
}

bool CurveGenerator::isValueWraps(void) const
{
	return valueWraps;
}

float CurveGenerator::getMaximumVelocity(void) const
{
	return maximumVelocity;
}

float CurveGenerator::getMaximumAcceleration(void) const
{
	return maximumAcceleration;
}

unsigned int CurveGenerator::getTicksPerAccelerationChange(void) const
{
	return ticksPerAccelerationChange;
}

float CurveGenerator::getValue(void) const
{
	return value;
}

float CurveGenerator::getVelocity(void) const
{
	return velocity;
}

float CurveGenerator::getAcceleration(void) const
{
	return acceleration;
}

unsigned int CurveGenerator::getTicks(void) const
{
	return ticks;
}

void CurveGenerator::advanceTick(void)
{
	if (++ticks > ticksPerAccelerationChange)
	{
		acceleration = distributor(randomDevice);
		ticks = 0;
	}
	velocity = std::clamp(velocity + acceleration, -maximumVelocity, maximumVelocity);
	value += velocity;
	if (valueWraps)
	{
		value = std::fmod(value - minimumValue, maximumValue - minimumValue) + minimumValue;
	}
	else
	{
		value = std::clamp(value, minimumValue, maximumValue);
	}
}

float CurveGenerator::getNextValue(void)
{
	advanceTick();
	return value;
}
