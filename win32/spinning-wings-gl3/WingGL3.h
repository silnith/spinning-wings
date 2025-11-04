#pragma once

#include <memory>

#include "Color.h"
#include "TransformedColorBuffer.h"
#include "TransformedVertexBuffer.h"

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
	template<typename T>
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
		/// <param name="deltaAngle">The additional angle around the central axis as the wing recedes into history.</param>
		/// <param name="deltaZ">The additional height along the central axis as the wing recedes into history.</param>
		Wing(T deltaAngle, T deltaZ) noexcept
			: deltaAngle{ deltaAngle }, deltaZ{ deltaZ }
		{}

		/// <summary>
		/// Creates a new wing with the provided parameters.
		/// </summary>
		/// <param name="vertexBuffer">The vertex buffer to reuse for this wing.</param>
		/// <param name="colorBuffer">The color buffer to reuse.</param>
		/// <param name="edgeColorBuffer">The edge color buffer to reuse.</param>
		/// <param name="deltaAngle">The additional angle around the central axis as the wing recedes into history.</param>
		/// <param name="deltaZ">The additional height along the central axis as the wing recedes into history.</param>
		Wing(std::shared_ptr<TransformedVertexBuffer> const& vertexBuffer,
			std::shared_ptr<TransformedColorBuffer> const& colorBuffer,
			std::shared_ptr<TransformedColorBuffer> const& edgeColorBuffer,
			T deltaAngle, T deltaZ) noexcept
			: vertexBuffer{ vertexBuffer },
			colorBuffer{ colorBuffer },
			edgeColorBuffer{ edgeColorBuffer },
			deltaAngle{ deltaAngle },
			deltaZ{ deltaZ }
		{}

		/// <summary>
		/// Returns the vertex buffer.  This is only used to hand off ownership
		/// to a new wing before this one is destroyed.
		/// </summary>
		/// <remarks>
		/// <para>
		/// This should be removed once I become comfortable with how to use
		/// unique pointers to transfer ownership out of an object.
		/// </para>
		/// </remarks>
		/// <returns>A shared pointer to the buffer.</returns>
		[[nodiscard]]
		inline std::shared_ptr<TransformedVertexBuffer> getVertexBuffer(void) const noexcept
		{
			return vertexBuffer;
		}

		/// <summary>
		/// Returns the color buffer.  This is only used to hand off ownership
		/// to a new wing before this one is destroyed.
		/// </summary>
		/// <remarks>
		/// <para>
		/// This should be removed once I become comfortable with how to use
		/// unique pointers to transfer ownership out of an object.
		/// </para>
		/// </remarks>
		/// <returns>A shared pointer to the buffer.</returns>
		[[nodiscard]]
		inline std::shared_ptr<TransformedColorBuffer> getColorBuffer(void) const noexcept
		{
			return colorBuffer;
		}

		/// <summary>
		/// Returns the edge color buffer.  This is only used to hand off ownership
		/// to a new wing before this one is destroyed.
		/// </summary>
		/// <remarks>
		/// <para>
		/// This should be removed once I become comfortable with how to use
		/// unique pointers to transfer ownership out of an object.
		/// </para>
		/// </remarks>
		/// <returns>A shared pointer to the buffer.</returns>
		[[nodiscard]]
		inline std::shared_ptr<TransformedColorBuffer> getEdgeColorBuffer(void) const noexcept
		{
			return edgeColorBuffer;
		}

		/// <summary>
		/// Returns the OpenGL vertex buffer identifier for this wing.
		/// The vertex buffer will be populated using transform feedback.
		/// </summary>
		/// <returns>The OpenGL vertex buffer identifier.</returns>
		[[nodiscard]]
		inline GLuint getVertexBufferId(void) const noexcept
		{
			return vertexBuffer->getId();
		}

		/// <summary>
		/// Returns the OpenGL color buffer identifier for this wing.
		/// </summary>
		/// <returns>The OpenGL color buffer identifier.</returns>
		[[nodiscard]]
		inline GLuint getColorBufferId(void) const noexcept
		{
			return colorBuffer->getId();
		}

		/// <summary>
		/// Returns the OpenGL edge color buffer identifier for this wing.
		/// </summary>
		/// <returns>The OpenGL edge color buffer identifier.</returns>
		[[nodiscard]]
		inline GLuint getEdgeColorBufferId(void) const noexcept
		{
			return edgeColorBuffer->getId();
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
		std::shared_ptr<TransformedVertexBuffer> vertexBuffer{ std::make_shared<TransformedVertexBuffer>() };
		std::shared_ptr<TransformedColorBuffer> colorBuffer{ std::make_shared<TransformedColorBuffer>() };
		std::shared_ptr<TransformedColorBuffer> edgeColorBuffer{ std::make_shared<TransformedColorBuffer>() };
		T const deltaAngle{ 15 };
		T const deltaZ{ 0.5 };
	};

}
