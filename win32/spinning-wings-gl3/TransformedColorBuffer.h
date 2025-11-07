#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// A class to encapsulate a GL buffer object as well as the critical
    /// parameters that define the data stored in it.  This object contains
    /// transformed colors.  It is populated using transform feedback,
    /// and is the source of rendering commands.
    /// </summary>
    class TransformedColorBuffer : public Buffer
    {
    public:
        /// <summary>
        /// The number of vertices in a single wing.
        /// </summary>
        static GLsizei constexpr numVertices{ 4 };

    private:
        /// <summary>
        /// The number of color components per vertex in the buffer.
        /// </summary>
        static GLint constexpr numCoordinatesPerVertex{ 3 };

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
        /// Constructs an array buffer object and allocates enough space in it
        /// to receive a color value for every vertex in <see cref="OriginalVertexBuffer"/>.
        /// </summary>
        TransformedColorBuffer(void);
        TransformedColorBuffer(TransformedColorBuffer const&) = delete;
        TransformedColorBuffer& operator=(TransformedColorBuffer const&) = delete;
        TransformedColorBuffer(TransformedColorBuffer&&) noexcept = delete;
        TransformedColorBuffer& operator=(TransformedColorBuffer&&) noexcept = delete;
        virtual ~TransformedColorBuffer(void) noexcept override = default;

    public:
        /// <summary>
        /// Sets this buffer as the array buffer pointed to for the specified
        /// vertex attribute.
        /// </summary>
        /// <param name="vertexAttributeLocation">The vertex attribute location to point to this buffer.</param>
        void SetForVertexAttribute(GLuint vertexAttributeLocation) const;
    };

}
