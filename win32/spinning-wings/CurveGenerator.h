#pragma once

#include <random>

/// <summary>
/// A class that produces a sequence of numbers that slowly shift within predefined boundaries.
/// </summary>
class CurveGenerator
{
public:
	CurveGenerator(void) = delete;
	~CurveGenerator(void) = default;
	CurveGenerator(CurveGenerator const& curveGenerator) = delete;
	CurveGenerator& operator=(CurveGenerator const& curveGenerator) = delete;
	CurveGenerator(CurveGenerator&& curveGenerator) = delete;
	CurveGenerator& operator=(CurveGenerator&& curveGenerator) = delete;
public:
	/// <summary>
	/// Creates a new curve generator.
	/// </summary>
	/// <param name="initialValue">the initial value of the curve</param>
	/// <param name="minimumValue">the minimum value of the curve</param>
	/// <param name="maximumValue">the maximum value of the curve</param>
	/// <param name="valueWraps">whether curve values map from the maximum value to the minimum value</param>
	/// <param name="maximumVelocity">the maximum velocity, or slope of the curve</param>
	/// <param name="maximumAcceleration">the maximum acceleration for curve changes</param>
	/// <param name="ticksPerAccelerationChange">the number of values to generate before the acceleration changes</param>
	explicit CurveGenerator(float initialValue, float minimumValue, float maximumValue, bool valueWraps,
		float maximumVelocity, float maximumAcceleration, unsigned int ticksPerAccelerationChange);

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
	/// <summary>
	/// Returns the minimum allowed value for the curve.
	/// </summary>
	/// <returns>the minimum curve value</returns>
	float getMinimumValue(void) const noexcept;
	
	/// <summary>
	/// Returns the maximum allowed value for the curve.
	/// </summary>
	/// <returns>the maximum curve value</returns>
	float getMaximumValue(void) const noexcept;
	
	/// <summary>
	/// Returns whether curve values wrap between maximum and minimum when they overflow.
	/// </summary>
	/// <returns><c>true</c> if exceeding the maximum value should wrap to the minimum value</returns>
	bool isValueWraps(void) const noexcept;
	
	/// <summary>
	/// Returns the maximum rate at which the curve value is allowed to change.
	/// </summary>
	/// <returns>the maximum difference between successive curve values</returns>
	float getMaximumVelocity(void) const noexcept;

	/// <summary>
	/// Returns the maximum acceleration for curve changes.
	/// </summary>
	/// <returns>the maximum amount the velocity is allowed to change between successive curve values</returns>
	float getMaximumAcceleration(void) const noexcept;

	/// <summary>
	/// Returns the number of curve values to return before changing the curve acceleration.
	/// </summary>
	/// <returns>the number of curve values that may be returned before the acceleration changes</returns>
	unsigned int getTicksPerAccelerationChange(void) const noexcept;

	/// <summary>
	/// Returns the current value of the curve.
	/// </summary>
	/// <returns>the current curve value</returns>
	float getValue(void) const noexcept;

	/// <summary>
	/// Returns the current velocity of the curve.  This is the amount the curve value has changed since the last value.
	/// </summary>
	/// <returns>the current curve velocity</returns>
	float getVelocity(void) const noexcept;

	/// <summary>
	/// Returns the current acceleration of the curve.  This is the amount that the curve velocity has changed since the last curve value.
	/// </summary>
	/// <returns>the current curve acceleration</returns>
	float getAcceleration(void) const noexcept;

	/// <summary>
	/// Returns the number of curve values retrieved since the last time the curve acceleration changed.
	/// </summary>
	/// <returns>the number of curve values since the last change in acceleration</returns>
	unsigned int getTicks(void) const noexcept;

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
	std::uniform_real_distribution<float> const distributor{ -maximumAcceleration, maximumAcceleration };

private:
	float value{ 0 };
	float velocity{ 0 };
	float acceleration{ 0 };
	unsigned int ticks{ ticksPerAccelerationChange };

	/// <summary>
	/// Advances the tick count, which may or may not trigger a change in acceleration.
	/// Then applies the current acceleration to the velocity,
	/// and aplies the current velocity to the current curve value.
	/// </summary>
	void advanceTick(void);

private:
	static std::random_device randomDevice;
};
