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
		/// <param name="deltaAngle">the additional angle around the central axis as the wing receds into history</param>
		/// <param name="deltaZ">the additional height along the central axis as the wing receds into history</param>
		Wing(unsigned int vertexBuffer,
			unsigned int colorBuffer,
			unsigned int edgeColorBuffer,
			float deltaAngle, float deltaZ) noexcept;

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
		/// Returns the additional angle around the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>the additional angle that the wing gains as it receds into history</returns>
		float getDeltaAngle(void) const noexcept;

		/// <summary>
		/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>the additional hight that the wing gains as it receds into history</returns>
		float getDeltaZ(void) const noexcept;

	private:
		unsigned int const vertexBuffer{ 0 };
		unsigned int const colorBuffer{ 0 };
		unsigned int const edgeColorBuffer{ 0 };
		float const deltaAngle{ 15 };
		float const deltaZ{ 0.5 };
	};

}
