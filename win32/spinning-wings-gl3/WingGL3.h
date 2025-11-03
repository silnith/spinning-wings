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
	template<typename ID, typename T>
	class Wing
	{
	public:
		Wing(void) noexcept = default;
		Wing(Wing const& wing) noexcept = default;
		Wing& operator=(Wing const& wing) noexcept = default;
		Wing(Wing&& wing) noexcept = default;
		Wing& operator=(Wing&& wing) noexcept = default;
		virtual ~Wing(void) noexcept = default;
	public:
		/// <summary>
		/// Creates a new wing with the provided parameters.
		/// </summary>
		/// <param name="vertexBuffer">The OpenGL vertex buffer for this wing.</param>
		/// <param name="colorBuffer">The OpenGL color buffer identifier.</param>
		/// <param name="edgeColorBuffer">The OpenGL edge color buffer identifier.</param>
		/// <param name="deltaAngle">The additional angle around the central axis as the wing recedes into history.</param>
		/// <param name="deltaZ">The additional height along the central axis as the wing recedes into history.</param>
		Wing(ID vertexBuffer,
			ID colorBuffer,
			ID edgeColorBuffer,
			T deltaAngle, T deltaZ) noexcept
			: vertexBuffer{ vertexBuffer }, colorBuffer{ colorBuffer }, edgeColorBuffer{ edgeColorBuffer },
			deltaAngle{ deltaAngle }, deltaZ{ deltaZ }
		{}

		/// <summary>
		/// Returns the OpenGL vertex buffer identifier for this wing.
		/// The vertex buffer will be populated using transform feedback.
		/// </summary>
		/// <returns>The OpenGL vertex buffer identifier.</returns>
		[[nodiscard]]
		inline ID getVertexBuffer(void) const noexcept
		{
			return vertexBuffer;
		}

		/// <summary>
		/// Returns the OpenGL color buffer identifier for this wing.
		/// </summary>
		/// <returns>The OpenGL color buffer identifier.</returns>
		[[nodiscard]]
		inline ID getColorBuffer(void) const noexcept
		{
			return colorBuffer;
		}

		/// <summary>
		/// Returns the OpenGL edge color buffer identifier for this wing.
		/// </summary>
		/// <returns>The OpenGL edge color buffer identifier.</returns>
		[[nodiscard]]
		inline ID getEdgeColorBuffer(void) const noexcept
		{
			return edgeColorBuffer;
		}

		/// <summary>
		/// Returns the additional angle around the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>The additional angle that the wing gains as it recedes into history.</returns>
		[[nodiscard]]
		inline T getDeltaAngle(void) const noexcept
		{
			return deltaAngle;
		}

		/// <summary>
		/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>The additional height that the wing gains as it recedes into history.</returns>
		[[nodiscard]]
		inline T getDeltaZ(void) const noexcept
		{
			return deltaZ;
		}

	private:
		ID const vertexBuffer{ 0 };
		ID const colorBuffer{ 0 };
		ID const edgeColorBuffer{ 0 };
		T const deltaAngle{ 15 };
		T const deltaZ{ 0.5 };
	};

}
