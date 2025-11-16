#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "Color.h"
#include "ArrayBuffer.h"

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
		virtual ~Wing(void) noexcept = default;
	public:
		/// <summary>
		/// Creates a new wing with the provided parameters.
		/// </summary>
		/// <param name="deltaAngle">The additional angle around the central axis as the wing recedes into history.</param>
		/// <param name="deltaZ">The additional height along the central axis as the wing recedes into history.</param>
		/// <param name="vertexBuffer">The vertex buffer to reuse for this wing.</param>
		/// <param name="colorBuffer">The color buffer to reuse.</param>
		/// <param name="edgeColorBuffer">The edge color buffer to reuse.</param>
		Wing(GLfloat deltaAngle, GLfloat deltaZ,
			std::shared_ptr<ArrayBuffer> const& vertexBuffer,
			std::shared_ptr<ArrayBuffer> const& colorBuffer,
			std::shared_ptr<ArrayBuffer> const& edgeColorBuffer) noexcept;

		/// <summary>
		/// Returns the additional angle around the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>The additional angle that the wing gains as it recedes into history.</returns>
		[[nodiscard]]
		GLfloat getDeltaAngle(void) const noexcept;

		/// <summary>
		/// Returns the additional distance up the central axis that each subsequent wing should be rendered.
		/// </summary>
		/// <returns>The additional height that the wing gains as it recedes into history.</returns>
		[[nodiscard]]
		GLfloat getDeltaZ(void) const noexcept;

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
		std::shared_ptr<ArrayBuffer> getVertexBuffer(void) const noexcept;

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
		std::shared_ptr<ArrayBuffer> getColorBuffer(void) const noexcept;

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
		std::shared_ptr<ArrayBuffer> getEdgeColorBuffer(void) const noexcept;

	private:
		GLfloat const deltaAngle{ 15 };
		GLfloat const deltaZ{ 0.5 };
		std::shared_ptr<ArrayBuffer> vertexBuffer{ nullptr };
		std::shared_ptr<ArrayBuffer> colorBuffer{ nullptr };
		std::shared_ptr<ArrayBuffer> edgeColorBuffer{ nullptr };
	};

}
