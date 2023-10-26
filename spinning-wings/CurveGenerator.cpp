#include "CurveGenerator.h"

#include <algorithm>

#include <cmath>

std::random_device CurveGenerator::randomDevice{ "default" };

CurveGenerator::CurveGenerator(float _initialValue, float _minimumValue, float _maximumValue, bool _valueWraps,
	float _maximumVelocity, float _maximumAcceleration, unsigned int _ticksPerAccelerationChange)
	: minimumValue{ _minimumValue }, maximumValue{ _maximumValue }, valueWraps{ _valueWraps },
	maximumVelocity{ _maximumVelocity }, maximumAcceleration{ _maximumAcceleration }, ticksPerAccelerationChange{ _ticksPerAccelerationChange },
	value{ _initialValue }
{}

CurveGenerator CurveGenerator::createGeneratorForAngles(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
{
	return CurveGenerator{ initialValue, 0.0f, 360.0f, true, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
}

CurveGenerator CurveGenerator::createGeneratorForColorComponents(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
{
	return CurveGenerator{ initialValue, 0.0f, 1.0f, false, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
}

float CurveGenerator::getMinimumValue(void) const noexcept
{
	return minimumValue;
}

float CurveGenerator::getMaximumValue(void) const noexcept
{
	return maximumValue;
}

bool CurveGenerator::isValueWraps(void) const noexcept
{
	return valueWraps;
}

float CurveGenerator::getMaximumVelocity(void) const noexcept
{
	return maximumVelocity;
}

float CurveGenerator::getMaximumAcceleration(void) const noexcept
{
	return maximumAcceleration;
}

unsigned int CurveGenerator::getTicksPerAccelerationChange(void) const noexcept
{
	return ticksPerAccelerationChange;
}

float CurveGenerator::getValue(void) const noexcept
{
	return value;
}

float CurveGenerator::getVelocity(void) const noexcept
{
	return velocity;
}

float CurveGenerator::getAcceleration(void) const noexcept
{
	return acceleration;
}

unsigned int CurveGenerator::getTicks(void) const noexcept
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
	return getValue();
}
