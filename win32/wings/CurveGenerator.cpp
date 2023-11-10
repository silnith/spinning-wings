#include "CurveGenerator.h"

#include <algorithm>

#include <cmath>

std::random_device silnith::CurveGenerator::randomDevice{ "default" };

silnith::CurveGenerator::CurveGenerator(float initialValue, float minimumValue, float maximumValue, bool valueWraps,
	float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
	: minimumValue{ minimumValue }, maximumValue{ maximumValue }, valueWraps{ valueWraps },
	maximumVelocity{ maximumVelocity }, maximumAcceleration{ maximumAcceleration }, ticksPerAccelerationChange{ ticksPerAccelerationChange },
	value{ initialValue }
{}

silnith::CurveGenerator silnith::CurveGenerator::createGeneratorForAngles(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
{
	return silnith::CurveGenerator{ initialValue, 0.0f, 360.0f, true, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
}

silnith::CurveGenerator silnith::CurveGenerator::createGeneratorForColorComponents(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange)
{
	return silnith::CurveGenerator{ initialValue, 0.0f, 1.0f, false, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
}

float silnith::CurveGenerator::getMinimumValue(void) const noexcept
{
	return minimumValue;
}

float silnith::CurveGenerator::getMaximumValue(void) const noexcept
{
	return maximumValue;
}

bool silnith::CurveGenerator::isValueWraps(void) const noexcept
{
	return valueWraps;
}

float silnith::CurveGenerator::getMaximumVelocity(void) const noexcept
{
	return maximumVelocity;
}

float silnith::CurveGenerator::getMaximumAcceleration(void) const noexcept
{
	return maximumAcceleration;
}

unsigned int silnith::CurveGenerator::getTicksPerAccelerationChange(void) const noexcept
{
	return ticksPerAccelerationChange;
}

float silnith::CurveGenerator::getValue(void) const noexcept
{
	return value;
}

float silnith::CurveGenerator::getVelocity(void) const noexcept
{
	return velocity;
}

float silnith::CurveGenerator::getAcceleration(void) const noexcept
{
	return acceleration;
}

unsigned int silnith::CurveGenerator::getTicks(void) const noexcept
{
	return ticks;
}

void silnith::CurveGenerator::setValue(float _value) noexcept
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

void silnith::CurveGenerator::setVelocity(float _velocity) noexcept
{
	velocity = std::clamp(_velocity, -maximumVelocity, maximumVelocity);
}

void silnith::CurveGenerator::advanceTick(void)
{
	if (++ticks > ticksPerAccelerationChange)
	{
		acceleration = distributor(randomDevice);
		ticks = 0;
	}
	setVelocity(velocity + acceleration);
	setValue(value + velocity);
}

float silnith::CurveGenerator::getNextValue(void)
{
	advanceTick();
	return getValue();
}
