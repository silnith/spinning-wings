#pragma once

#include "Color.h"

namespace silnith::wings
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
		/// Creates a new wing with the provided OpenGL display list identifier.
		/// </summary>
		/// <param name="displayList">the OpenGL display list identifier used for the rendering list for this wing</param>
		Wing(ID displayList) noexcept
			: displayList{ displayList }
		{}

		/// <summary>
		/// Creates a new wing with the provided parameters.
		/// </summary>
		/// <param name="displayList">the OpenGL display list identifier used for the rendering list for this wing</param>
		/// <param name="radius">the radius from the central axis</param>
		/// <param name="angle">the angle around the central axis</param>
		/// <param name="deltaAngle">the additional angle around the central axis as the wing receds into history</param>
		/// <param name="deltaZ">the additional height along the central axis as the wing receds into history</param>
		/// <param name="roll">the roll of the wing</param>
		/// <param name="pitch">the pitch of the wing</param>
		/// <param name="yaw">the yaw of the wing</param>
		/// <param name="color">the color of the wing</param>
		/// <param name="edgeColor">the color of the edge of the wing</param>
		Wing(ID displayList,
			T radius, T angle,
			T deltaAngle, T deltaZ,
			T roll, T pitch, T yaw,
			Color<T> const& color,
			Color<T> const& edgeColor) noexcept
			: displayList{ displayList },
			radius{ radius }, angle{ angle }, deltaAngle{ deltaAngle }, deltaZ{ deltaZ },
			roll{ roll }, pitch{ pitch }, yaw{ yaw },
			color{ color }, edgeColor{ edgeColor }
		{}

		/// <summary>
		/// Returns the OpenGL display list identifier that renders this wing.
		/// </summary>
		/// <returns>the OpenGL display list identifier</returns>
		[[nodiscard]]
		inline ID getGLDisplayList(void) const noexcept
		{
			return displayList;
		}

		/// <summary>
		/// Returns the distance of the wing from the central axis around which they all rotate.
		/// </summary>
		/// <returns>the distance of the wing from the central axis</returns>
		[[nodiscard]]
		inline T getRadius(void) const noexcept
		{
			return radius;
		}

		/// <summary>
		/// Returns the angle of the wing around the central axis.
		/// </summary>
		/// <remarks>
		/// <para>
		/// All angles are in degrees.
		/// </para>
		/// </remarks>
		/// <returns>the angle of the wing in relation to the central axis</returns>
		[[nodiscard]]
		inline T getAngle(void) const noexcept
		{
			return angle;
		}

		/// <summary>
		/// Returns the additional angle around the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>the additional angle that the wing gains as it receds into history</returns>
		[[nodiscard]]
		inline T getDeltaAngle(void) const noexcept
		{
			return deltaAngle;
		}

		/// <summary>
		/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>the additional hight that the wing gains as it receds into history</returns>
		[[nodiscard]]
		inline T getDeltaZ(void) const noexcept
		{
			return deltaZ;
		}

		/// <summary>
		/// Returns the roll of the wing.  This is specific to the wing itself.
		/// </summary>
		/// <returns>the roll of the wing in degrees</returns>
		[[nodiscard]]
		inline T getRoll(void) const noexcept
		{
			return roll;
		}

		/// <summary>
		/// Returns the pitch of the wing.
		/// </summary>
		/// <returns>the pitch of the wing in degrees</returns>
		[[nodiscard]]
		inline T getPitch(void) const noexcept
		{
			return pitch;
		}

		/// <summary>
		/// Returns the yaw of the wing.
		/// </summary>
		/// <returns>the yaw of the wing in degrees</returns>
		[[nodiscard]]
		inline T getYaw(void) const noexcept
		{
			return yaw;
		}

		/// <summary>
		/// Returns the color of the wing.
		/// </summary>
		/// <returns>the color of the wing</returns>
		[[nodiscard]]
		inline Color<T> const& getColor(void) const noexcept
		{
			return color;
		}

		/// <summary>
		/// Returns the color of the wing edge.
		/// </summary>
		/// <returns>the color of the edge of the wing</returns>
		[[nodiscard]]
		inline Color<T> const& getEdgeColor(void) const noexcept
		{
			return edgeColor;
		}

	private:
		ID const displayList{ 0 };
		T const radius{ 10 };
		T const angle{ 0 };
		T const deltaAngle{ 15 };
		T const deltaZ{ 0.5 };
		T const roll{ 0 };
		T const pitch{ 0 };
		T const yaw{ 0 };
		Color<T> const color{ Color<T>::BLACK };
		Color<T> const edgeColor{ Color<T>::WHITE };
	};

}
