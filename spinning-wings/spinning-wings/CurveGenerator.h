#pragma once

#include <random>

/// <summary>
/// A class that produces a sequence of numbers that slowly shift within predefined boundaries.
/// </summary>
class CurveGenerator
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="_initialValue">the initial value of the curve</param>
	/// <param name="_minimumValue">the minimum value of the curve</param>
	/// <param name="_maximumValue">the maximum value of the curve</param>
	/// <param name="_valueWraps">whether curve values map from the maximum value to the minimum value</param>
	/// <param name="_maximumVelocity">the maximum velocity, or slope of the curve</param>
	/// <param name="_maximumAcceleration">the maximum acceleration for curve changes</param>
	/// <param name="_ticksPerAccelerationChange">the number of values to generate before the acceleration changes</param>
	CurveGenerator(float _initialValue, float _minimumValue, float _maximumValue, bool _valueWraps,
		float _maximumVelocity, float _maximumAcceleration, unsigned int _ticksPerAccelerationChange);

	/// <summary>
	/// Returns a CurveGenerator for angles.
	/// </summary>
	/// <param name="initialValue">the initial angle</param>
	/// <param name="maximumVelocity">the maximum angle change per tick</param>
	/// <param name="maximumAcceleration">the maximum acceleration</param>
	/// <param name="ticksPerAccelerationChange">the number of values to generate before the acceleration changes</param>
	/// <returns>a CurveGenerator that returns values that can be used as angles</returns>
	static CurveGenerator createGeneratorForAngles(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange);

	/// <summary>
	/// Returns a CurveGenerator for color component values.  In other words, red, green, or blue.
	/// </summary>
	/// <param name="initialValue">the initial color value</param>
	/// <param name="maximumVelocity">the maximum color change per tick</param>
	/// <param name="maximumAcceleration">the maximum acceleration</param>
	/// <param name="ticksPerAccelerationChange">the number of values to generate before the acceleration changes</param>
	/// <returns>a CurveGenerator that returns values specific for color components</returns>
	static CurveGenerator createGeneratorForColorComponents(float initialValue, float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange);

public:
	float getMinimumValue(void) const;
	float getMaximumValue(void) const;
	bool isValueWraps(void) const;
	float getMaximumVelocity(void) const;
	float getMaximumAcceleration(void) const;
	unsigned int getTicksPerAccelerationChange(void) const;

	float getValue(void) const;
	float getVelocity(void) const;
	float getAcceleration(void) const;
	unsigned int getTicks(void) const;

	/// <summary>
	/// Advances the tick count, applies velocity and acceleration, and returns the next value for the random curve.
	/// </summary>
	/// <returns>the next value of the curve</returns>
	float getNextValue(void);

private:
	float const minimumValue;
	float const maximumValue;
	bool const valueWraps;

	float const maximumVelocity;
	float const maximumAcceleration;
	unsigned int const ticksPerAccelerationChange;
	std::uniform_real_distribution<float> const distributor;

private:
	float value;
	float velocity;
	float acceleration;
	unsigned int ticks;

	void advanceTick(void);

private:
	static std::random_device randomDevice;
};
