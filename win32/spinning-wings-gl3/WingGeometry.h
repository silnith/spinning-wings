#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <array>
#include <memory>

#include "ArrayBuffer.h"
#include "ElementArrayBuffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// An encapsulated central location for handling the wing geometry and
    /// various buffers that hold it.
    /// </summary>
    class WingGeometry
    {
    public:
        explicit WingGeometry(void);
        WingGeometry(WingGeometry const&) = default;
        WingGeometry& operator=(WingGeometry const&) = default;
        WingGeometry(WingGeometry&&) noexcept = default;
        WingGeometry& operator=(WingGeometry&&) noexcept = default;
        ~WingGeometry(void) noexcept = default;

    public:
        /// <summary>
        /// Returns the number of vertices in the wing.
        /// This is the number of (x, y, z, w) coordinate tuples, and is
        /// independent of how they are connected into primitives.
        /// </summary>
        /// <returns>The total number of vertices that make up a wing.</returns>
        [[nodiscard]]
        GLsizei getNumVertices(void) const;

        /// <summary>
        /// Uses the original, untransformed wing vertex coordinates for the
        /// specified vertex attribute location.
        /// </summary>
        /// <param name="attributeLocation">The index of the generic vertex attribute.</param>
        void UseForVertexAttribute(GLuint attributeLocation) const;

        /// <summary>
        /// Use the indices into the wing vertex coordinate buffers as the
        /// <c>GL_ELEMENT_ARRAY_BUFFER</c>.
        /// </summary>
        void UseElementArrayBuffer(void) const;

        /// <summary>
        /// Draws the wing as a cloud of unconnected points.
        /// This is used for the wing transformation GLSL program running under
        /// transform feedback to transform the vertices for later rendering.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This expects that <see cref="UseForVertexAttribute"/> be called to
        /// set the source of vertex coordinates, or another buffer used that
        /// contains the same number of vertices.
        /// </para>
        /// </remarks>
        void RenderAsPoints(void) const;

        /// <summary>
        /// Draws the wing as solid polygons.
        /// This is used for the final rendering to the screen.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This expects that <see cref="UseForVertexAttribute"/> be called to
        /// set the source of vertex coordinates, or another buffer used that
        /// contains the same number of vertices.
        /// </para>
        /// <para>
        /// This also expects that the index buffer be bound using
        /// <see cref="UseElementArrayBuffer"/>.
        /// </para>
        /// </remarks>
        /// <seealso cref="UseElementArrayBuffer"/>
        void RenderAsPolygons(void) const;

        /// <summary>
        /// Draws the wing as an outline.
        /// This is used for the final rendering to the screen.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This expects that <see cref="UseForVertexAttribute"/> be called to
        /// set the source of vertex coordinates, or another buffer used that
        /// contains the same number of vertices.
        /// </para>
        /// <para>
        /// This also expects that the index buffer be bound using
        /// <see cref="UseElementArrayBuffer"/>.
        /// </para>
        /// </remarks>
        /// <seealso cref="UseElementArrayBuffer"/>
        void RenderAsOutline(void) const;

    private:
        /// <summary>
        /// The original untransformed vertex coordinates.
        /// </summary>
        ArrayBuffer const vertexArrayBuffer;

        /// <summary>
        /// The indices into <see cref="vertexArrayBuffer"/> that are assembled into primitives.
        /// </summary>
        ElementArrayBuffer const elementArrayBuffer;
    };

}
