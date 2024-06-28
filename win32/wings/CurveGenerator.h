#pragma once

#include <algorithm>
#include <concepts>
#include <random>

#include <cmath>

namespace silnith::wings
{

	/// <summary>
	/// A class that produces a sequence of numbers that slowly shift within predefined boundaries.
	/// </summary>
	template<std::floating_point T>
	class CurveGenerator
	{
	public:
		CurveGenerator(void) = delete;
		CurveGenerator(CurveGenerator const& curveGenerator) = delete;
		CurveGenerator& operator=(CurveGenerator const& curveGenerator) = delete;
		CurveGenerator(CurveGenerator&& curveGenerator) = delete;
		CurveGenerator& operator=(CurveGenerator&& curveGenerator) = delete;
		virtual ~CurveGenerator(void) noexcept = default;
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
		explicit CurveGenerator(T initialValue, T minimumValue, T maximumValue, bool valueWraps,
			T maximumVelocity, T maximumAcceleration, unsigned int ticksPerAccelerationChange)
			: minimumValue{ minimumValue }, maximumValue{ maximumValue }, valueWraps{ valueWraps },
			maximumVelocity{ maximumVelocity }, maximumAcceleration{ maximumAcceleration }, ticksPerAccelerationChange{ ticksPerAccelerationChange },
			value{ initialValue }
		{}

		/// <summary>
		/// Returns a CurveGenerator for angles.
		/// </summary>
		/// <param name="initialValue">the initial angle</param>
		/// <param name="maximumVelocity">the maximum angle change per tick</param>
		/// <param name="maximumAcceleration">the maximum acceleration</param>
		/// <param name="ticksPerAccelerationChange">the number of values to generate before the acceleration changes</param>
		/// <returns>a CurveGenerator that returns values that can be used as angles</returns>
		[[nodiscard]]
		static CurveGenerator<T> createGeneratorForAngles(T initialValue, T maximumVelocity, T maximumAcceleration, unsigned int ticksPerAccelerationChange)
		{
			return CurveGenerator<T>{ initialValue, 0, 360, true, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
		}

		/// <summary>
		/// Returns a CurveGenerator for color component values.  In other words, red, green, or blue.
		/// </summary>
		/// <param name="initialValue">the initial color value</param>
		/// <param name="maximumVelocity">the maximum color change per tick</param>
		/// <param name="maximumAcceleration">the maximum acceleration</param>
		/// <param name="ticksPerAccelerationChange">the number of values to generate before the acceleration changes</param>
		/// <returns>a CurveGenerator that returns values specific for color components</returns>
		[[nodiscard]]
		static CurveGenerator<T> createGeneratorForColorComponents(T initialValue, T maximumVelocity, T maximumAcceleration, unsigned int ticksPerAccelerationChange)
		{
			return CurveGenerator<T>{ initialValue, 0, 1, false, maximumVelocity, maximumAcceleration, ticksPerAccelerationChange };
		}

	public:
		/// <summary>
		/// Returns the minimum allowed value for the curve.
		/// </summary>
		/// <returns>the minimum curve value</returns>
		[[nodiscard]]
		inline T getMinimumValue(void) const noexcept
		{
			return minimumValue;
		}

		/// <summary>
		/// Returns the maximum allowed value for the curve.
		/// </summary>
		/// <returns>the maximum curve value</returns>
		[[nodiscard]]
		inline T getMaximumValue(void) const noexcept
		{
			return maximumValue;
		}

		/// <summary>
		/// Returns whether curve values wrap between maximum and minimum when they overflow.
		/// </summary>
		/// <returns><c>true</c> if exceeding the maximum value should wrap to the minimum value</returns>
		[[nodiscard]]
		inline bool isValueWraps(void) const noexcept
		{
			return valueWraps;
		}

		/// <summary>
		/// Returns the maximum rate at which the curve value is allowed to change.
		/// </summary>
		/// <returns>the maximum difference between successive curve values</returns>
		[[nodiscard]]
		inline T getMaximumVelocity(void) const noexcept
		{
			return maximumVelocity;
		}

		/// <summary>
		/// Returns the maximum acceleration for curve changes.
		/// </summary>
		/// <returns>the maximum amount the velocity is allowed to change between successive curve values</returns>
		[[nodiscard]]
		inline T getMaximumAcceleration(void) const noexcept
		{
			return maximumAcceleration;
		}

		/// <summary>
		/// Returns the number of curve values to return before changing the curve acceleration.
		/// </summary>
		/// <returns>the number of curve values that may be returned before the acceleration changes</returns>
		[[nodiscard]]
		inline unsigned int getTicksPerAccelerationChange(void) const noexcept
		{
			return ticksPerAccelerationChange;
		}

		/// <summary>
		/// Returns the current value of the curve.
		/// </summary>
		/// <returns>the current curve value</returns>
		[[nodiscard]]
		inline T getValue(void) const noexcept
		{
			return value;
		}

		/// <summary>
		/// Returns the current velocity of the curve.  This is the amount the curve value has changed since the last value.
		/// </summary>
		/// <returns>the current curve velocity</returns>
		[[nodiscard]]
		inline T getVelocity(void) const noexcept
		{
			return velocity;
		}

		/// <summary>
		/// Returns the current acceleration of the curve.  This is the amount that the curve velocity has changed since the last curve value.
		/// </summary>
		/// <returns>the current curve acceleration</returns>
		[[nodiscard]]
		inline T getAcceleration(void) const noexcept
		{
			return acceleration;
		}

		/// <summary>
		/// Returns the number of curve values retrieved since the last time the curve acceleration changed.
		/// </summary>
		/// <returns>the number of curve values since the last change in acceleration</returns>
		[[nodiscard]]
		inline unsigned int getTicks(void) const noexcept
		{
			return ticks;
		}

		/// <summary>
		/// Advances the tick count, applies velocity and acceleration, and returns the next value for the random curve.
		/// </summary>
		/// <returns>the next value of the curve</returns>
		[[nodiscard]]
		T getNextValue(void)
		{
			advanceTick();
			return getValue();
		}

	private:
		T const minimumValue;
		T const maximumValue;
		bool const valueWraps;

		T const maximumVelocity;
		T const maximumAcceleration;
		unsigned int const ticksPerAccelerationChange;
		std::uniform_real_distribution<T> const distributor{ -maximumAcceleration, maximumAcceleration };

	private:
		T value{ 0 };
		T velocity{ 0 };
		T acceleration{ 0 };
		unsigned int ticks{ ticksPerAccelerationChange };

		void setValue(T _value)
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

		void setVelocity(T _velocity)
		{
			velocity = std::clamp(_velocity, -maximumVelocity, maximumVelocity);
		}

		/// <summary>
		/// Advances the tick count, which may or may not trigger a change in acceleration.
		/// Then applies the current acceleration to the velocity,
		/// and aplies the current velocity to the current curve value.
		/// </summary>
		void advanceTick(void)
		{
			if (++ticks > ticksPerAccelerationChange)
			{
				acceleration = distributor(randomDevice);
				ticks = 0;
			}
			setVelocity(velocity + acceleration);
			setValue(value + velocity);
		}

	private:
		static std::random_device randomDevice;
	};

	template<std::floating_point T>
	std::random_device CurveGenerator<T>::randomDevice{ "default" };

}
