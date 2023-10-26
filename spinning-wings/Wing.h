#pragma once

#include "Color.h"

/// <summary>
/// A single quad spinning around the central axis.
/// </summary>
/// <remarks>
/// <para>
/// The graphics hack is composed of several wings rendered in relation to a central axis.
/// The axis functions like a pole sticking straight up.
/// </para>
/// <para>
/// Conceptually there is only one wing that slowly changes over time.
/// However several copies of the wing are drawn, each copy being
/// the state of the wing in the previous render tick.  Therefore
/// if there are ten wings drawn, the top is exactly the wing that
/// was at the bottom ten ticks before, and the bottom is the most
/// recent wing generated.
/// </para>
/// <para>
/// All angles are in degrees.
/// </para>
/// </remarks>
class Wing
{
public:
	/*
	 * C++ generated members:
	 *  - default constructor
	 *  - destructor
	 *  - copy constructor & assignment operator
	 *  - move constructor & assignment operator
	 */
	
	/// <summary>
	/// Default constructor generated by the compiler.
	/// </summary>
	Wing(void) noexcept = default;
	/// <summary>
	/// Copy constructor generated by the compiler.
	/// </summary>
	/// <param name="wing">the wing to copy</param>
	Wing(Wing const& wing) noexcept = default;
	/// <summary>
	/// Copy assignment operator generated by the compiler.
	/// </summary>
	/// <param name="wing">the wing to copy</param>
	/// <returns>this wing</returns>
	Wing& operator=(Wing const& wing) noexcept = default;
	/// <summary>
	/// Move constructor generated by the compiler.
	/// </summary>
	/// <param name="wing">the wing to move</param>
	Wing(Wing&& wing) noexcept = default;
	/// <summary>
	/// Move assignment operator generated by the compiler.
	/// </summary>
	/// <param name="wing">the wing to move</param>
	/// <returns>this wing</returns>
	Wing& operator=(Wing&& wing) noexcept = default;
public:
	/// <summary>
	/// Creates a new wing with the provided parameters.
	/// </summary>
	/// <param name="_radius">the radius from the central axis</param>
	/// <param name="_angle">the angle around the central axis</param>
	/// <param name="_angleDelta">the additional angle around the central axis as the wing receds into history</param>
	/// <param name="_deltaZ">the additional height along the cenetral axis as the wing receds into history</param>
	/// <param name="_roll">the roll of the wing</param>
	/// <param name="_pitch">the pitch of the wing</param>
	/// <param name="_yaw">the yaw of the wing</param>
	/// <param name="_color">the color of the wing</param>
	/// <param name="_edgeColor">the color of the edge of the wing</param>
	Wing(float _radius, float _angle, float _angleDelta, float _deltaZ,
		float _roll, float _pitch, float _yaw,
		Color const& _color,
		Color const& _edgeColor) noexcept;

	/// <summary>
	/// Returns the distance of the wing from the central axis around which they all rotate.
	/// </summary>
	/// <returns>the distance of the wing from the central axis</returns>
	float getRadius(void) const noexcept;

	/// <summary>
	/// Returns the angle of the wing around the central axis.
	/// </summary>
	/// <remarks>
	/// <para>
	/// All angles are in degrees.
	/// </para>
	/// </remarks>
	/// <returns>the angle of the wing in relation to the central axis</returns>
	float getAngle(void) const noexcept;

	/// <summary>
	/// Returns the additional angle around the central axis that each subsequent wing should be rendered.
	/// </summary>
	/// <returns>the additional angle that the wing gains as it receeds into history</returns>
	float getDeltaAngle(void) const noexcept;

	/// <summary>
	/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
	/// </summary>
	/// <returns>the additional hight that the wing gains as it receeds into history</returns>
	float getZDelta(void) const noexcept;

	/// <summary>
	/// Returns the roll of the wing.  This is specific to the wing itself.
	/// </summary>
	/// <returns>the roll of the wing in degrees</returns>
	float getRoll(void) const noexcept;

	/// <summary>
	/// Returns the pitch of the wing.
	/// </summary>
	/// <returns>the pitch of the wing in degrees</returns>
	float getPitch(void) const noexcept;

	/// <summary>
	/// Returns the yaw of the wing.
	/// </summary>
	/// <returns>the yaw of the wing in degrees</returns>
	float getYaw(void) const noexcept;

	/// <summary>
	/// Returns the color of the wing.
	/// </summary>
	/// <returns>the color of the wing</returns>
	Color const& getColor(void) const noexcept;

	/// <summary>
	/// Returns the color of the wing edge.
	/// </summary>
	/// <returns>the color of the edge of the wing</returns>
	Color const& getEdgeColor(void) const noexcept;

private:
	float const radius{ 10 };
	float const angle{ 0 };
	float const angleDelta{ 15 };
	float const deltaZ{ 0.5 };
	float const roll{ 0 };
	float const pitch{ 0 };
	float const yaw{ 0 };
	Color const color{ Color::BLACK };
	Color const edgeColor{ Color::WHITE };
};
