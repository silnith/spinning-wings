#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "Color.h"
#include "ArrayBuffer.h"
#include "WingTransformFeedback.h"

namespace silnith::wings::gl4
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
		/// <summary>
		/// Default constructor is required by the STL containers.
		/// </summary>
		explicit Wing(void) = default;

		/// <summary>
		/// Creates a new wing with the provided parameters.
		/// </summary>
		/// <param name="deltaAngle">The additional angle around the central axis as the wing recedes into history.</param>
		/// <param name="deltaZ">The additional height along the central axis as the wing recedes into history.</param>
		explicit Wing(
			std::shared_ptr<WingTransformFeedback const> transformFeedbackObject,
			GLfloat deltaAngle, GLfloat deltaZ) noexcept
			: transformFeedbackObject{ transformFeedbackObject },
			deltaAngle{ deltaAngle }, deltaZ{ deltaZ }
		{}

	public:
		Wing(Wing const& wing) = default;
		Wing& operator=(Wing const& wing) = default;
		Wing(Wing&& wing) noexcept = default;
		Wing& operator=(Wing&& wing) noexcept = default;
		virtual ~Wing(void) noexcept = default;

	public:
		/// <summary>
		/// Returns the OpenGL transform feedback object identifier for this wing.
		/// </summary>
		/// <returns>The OpenGL transform feedback object identifier.</returns>
		[[nodiscard]]
		inline std::shared_ptr<WingTransformFeedback const> getTransformFeedbackObject(void) const noexcept
		{
			return transformFeedbackObject;
		}

		/// <summary>
		/// Returns the additional angle around the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>The additional angle that the wing gains as it recedes into history.</returns>
		[[nodiscard]]
		inline GLfloat getDeltaAngle(void) const noexcept
		{
			return deltaAngle;
		}

		/// <summary>
		/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>The additional hight that the wing gains as it recedes into history.</returns>
		[[nodiscard]]
		inline GLfloat getDeltaZ(void) const noexcept
		{
			return deltaZ;
		}

	private:
		std::shared_ptr<WingTransformFeedback const> transformFeedbackObject{ nullptr };
		GLfloat const deltaAngle{ 15 };
		GLfloat const deltaZ{ 0.5 };
	};

}
