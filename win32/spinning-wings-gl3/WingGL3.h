#pragma once

#include "Color.h"

namespace silnith::wings::gl3
{

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
		Wing(void) noexcept = default;
		Wing(Wing const& wing) noexcept = default;
		Wing& operator=(Wing const& wing) noexcept = default;
		Wing(Wing&& wing) noexcept = default;
		Wing& operator=(Wing&& wing) noexcept = default;
		~Wing(void) noexcept = default;
	public:
		/// <summary>
		/// Creates a new wing with the provided parameters.
		/// </summary>
		/// <param name="vertexBuffer">the OpenGL vertex buffer for this wing</param>
		/// <param name="colorBuffer">the OpenGL color buffer identifier</param>
		/// <param name="edgeColorBuffer">the OpenGL edge color buffer identifier</param>
		/// <param name="radius">the radius from the central axis</param>
		/// <param name="angle">the angle around the central axis</param>
		/// <param name="deltaAngle">the additional angle around the central axis as the wing receds into history</param>
		/// <param name="deltaZ">the additional height along the central axis as the wing receds into history</param>
		/// <param name="roll">the roll of the wing</param>
		/// <param name="pitch">the pitch of the wing</param>
		/// <param name="yaw">the yaw of the wing</param>
		/// <param name="color">the color of the wing</param>
		/// <param name="edgeColor">the color of the edge of the wing</param>
		Wing(unsigned int vertexBuffer,
			unsigned int colorBuffer,
			unsigned int edgeColorBuffer,
			float radius, float angle,
			float deltaAngle, float deltaZ,
			float roll, float pitch, float yaw,
			Color const& color,
			Color const& edgeColor) noexcept;

		/// <summary>
		/// Returns the OpenGL vertex buffer identifier for this wing.
		/// The vertex buffer will be populated using transform feedback.
		/// </summary>
		/// <returns>the OpenGL vertex buffer identifier</returns>
		unsigned int getVertexBuffer(void) const noexcept;

		/// <summary>
		/// Returns the OpenGL color buffer identifier for this wing.
		/// </summary>
		/// <returns>the OpenGL color buffer identifier</returns>
		unsigned int getColorBuffer(void) const noexcept;

		/// <summary>
		/// Returns the OpenGL edge color buffer identifier for this wing.
		/// </summary>
		/// <returns>the OpenGL edge color buffer identifier</returns>
		unsigned int getEdgeColorBuffer(void) const noexcept;

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
		/// <returns>the additional angle that the wing gains as it receds into history</returns>
		float getDeltaAngle(void) const noexcept;

		/// <summary>
		/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>the additional hight that the wing gains as it receds into history</returns>
		float getDeltaZ(void) const noexcept;

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
		unsigned int const vertexBuffer{ 0 };
		unsigned int const colorBuffer{ 0 };
		unsigned int const edgeColorBuffer{ 0 };
		float const radius{ 10 };
		float const angle{ 0 };
		float const deltaAngle{ 15 };
		float const deltaZ{ 0.5 };
		float const roll{ 0 };
		float const pitch{ 0 };
		float const yaw{ 0 };
		Color const color{ Color::BLACK };
		Color const edgeColor{ Color::WHITE };
	};

}
