#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// A class to encapsulate a GL buffer object as well as the critical
    /// parameters that define the data stored in it.  This object contains
    /// the original untransformed vertices for a wing.
    /// </summary>
    class OriginalVertexBuffer : public Buffer
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
        static GLint constexpr numCoordinatesPerVertex{ 2 };

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
        /// Constructs the buffer object and populates it with the vertex
        /// coordinates for the wing.
        /// </summary>
        OriginalVertexBuffer(void);
        OriginalVertexBuffer(OriginalVertexBuffer const&) = delete;
        OriginalVertexBuffer& operator=(OriginalVertexBuffer const&) = delete;
        OriginalVertexBuffer(OriginalVertexBuffer&&) noexcept = delete;
        OriginalVertexBuffer& operator=(OriginalVertexBuffer&&) noexcept = delete;
        virtual ~OriginalVertexBuffer(void) noexcept override = default;

    public:
        /// <summary>
        /// Sets this buffer as the array buffer pointed to for the specified
        /// vertex attribute.
        /// </summary>
        /// <param name="vertexAttributeLocation">The vertex attribute location to point to this buffer.</param>
        void SetForVertexAttribute(GLuint vertexAttributeLocation) const;
    };

}
