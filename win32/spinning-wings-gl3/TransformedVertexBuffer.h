#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// A class to encapsulate a GL buffer object as well as the critical
    /// parameters that define the data stored in it.  This object contains
    /// transformed vertices.  It is populated using transform feedback,
    /// and is the source of rendering commands.
    /// </summary>
    class TransformedVertexBuffer : public Buffer
    {
    public:
        /// <summary>
        /// The number of vertices in a single wing.
        /// </summary>
        static GLsizei constexpr numVertices{ 4 };

    private:
        /// <summary>
        /// The number of coordinates per vertex in the buffer.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Since this is populated using transform feedback, it will receive
        /// the full (x, y, z, w) coordinates calculated by the vertex shader.
        /// </para>
        /// </remarks>
        static GLint constexpr numCoordinatesPerVertex{ 4 };

        /// <summary>
        /// The data type of buffer elements.
        /// </summary>
        static GLenum constexpr vertexCoordinateDataType{ GL_FLOAT };

        /// <summary>
        /// The byte offset between consecutive vertices in the buffer.
        /// </summary>
        static GLsizei constexpr vertexStride{ 0 };

    public:
        /// <summary>
        /// Constructs an element array buffer and allocates enough space in it
        /// to receive vertex coordinates (via transform feedback) for every
        /// vertex in <see cref="OriginalVertexBuffer"/>.
        /// </summary>
        TransformedVertexBuffer(void);
        TransformedVertexBuffer(TransformedVertexBuffer const&) = delete;
        TransformedVertexBuffer& operator=(TransformedVertexBuffer const&) = delete;
        TransformedVertexBuffer(TransformedVertexBuffer&&) noexcept = delete;
        TransformedVertexBuffer& operator=(TransformedVertexBuffer&&) noexcept = delete;
        virtual ~TransformedVertexBuffer(void) noexcept override = default;

    public:
        /// <summary>
        /// Sets this buffer as the array buffer pointed to for the specified
        /// vertex attribute.
        /// </summary>
        /// <param name="vertexAttributeLocation">The vertex attribute location to point to this buffer.</param>
        void SetAsVertexAttribute(GLuint vertexAttributeLocation) const;
    };

}
