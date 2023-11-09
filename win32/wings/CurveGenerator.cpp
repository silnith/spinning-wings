#include "CurveGenerator.h"

#include <algorithm>

#include <cmath>

std::random_device CurveGenerator::randomDevice{ "default" };

CurveGenerator::CurveGenerator(float initialValue, float minimumValue, float maximumValue, bool valueWraps,
	float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
	: minimumValue{ minimumValue }, maximumValue{ maximumValue }, valueWraps{ valueWraps },
	maximumVelocity{ maximumVelocity }, maximumAcceleration{ maximumAcceleration }, ticksPerAccelerationChange{ ticksPerAccelerationChange },
	value{ initialValue }
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

void CurveGenerator::setValue(float _value) noexcept
{
	if (valueWraps)
	{
		value = std::fmod(_value - minimumValue, maximumValue - minimumValue) + minimumValue;
	}
	else
	{
		value = std::clamp(_value, minimumValue, maximumValue);
	}
}

void CurveGenerator::setVelocity(float _velocity) noexcept
{
	velocity = std::clamp(_velocity, -maximumVelocity, maximumVelocity);
}

void CurveGenerator::advanceTick(void)
{
	if (++ticks > ticksPerAccelerationChange)
	{
		acceleration = distributor(randomDevice);
		ticks = 0;
	}
	setVelocity(velocity + acceleration);
	setValue(value + velocity);
}

float CurveGenerator::getNextValue(void)
{
	advanceTick();
	return getValue();
}
