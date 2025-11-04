#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include <cstddef>

namespace silnith::wings::gl3
{

    /// <summary>
    /// A class to encapsulate a GL buffer object as well as the critical
    /// parameters that define the data stored in it.  This object contains
    /// transformed vertices.  It is populated using transform feedback,
    /// and is the source of rendering commands.
    /// </summary>
    class TransformedVertexBuffer
    {
    public:
        /// <summary>
        /// The number of vertices in a single wing.
        /// </summary>
        static GLsizei constexpr numVertices{ 4 };

        /// <summary>
        /// The number of coordinates per vertex in the buffer.
        /// </summary>
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
        TransformedVertexBuffer(void)
        {
            glGenBuffers(1, &id);

            // This static_assert is here as a reminder that the sizeof(GLfloat) must match.
            GLsizeiptr constexpr transformedVerticesDataSize{ sizeof(GLfloat) * numCoordinatesPerVertex * numVertices };
            static_assert(vertexCoordinateDataType == GL_FLOAT);

            glBindBuffer(GL_ARRAY_BUFFER, id);
            glBufferData(GL_ARRAY_BUFFER, transformedVerticesDataSize, nullptr, GL_STREAM_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        TransformedVertexBuffer(TransformedVertexBuffer const&) = delete;
        TransformedVertexBuffer& operator=(TransformedVertexBuffer const&) = delete;
        TransformedVertexBuffer(TransformedVertexBuffer&&) noexcept = delete;
        TransformedVertexBuffer& operator=(TransformedVertexBuffer&&) noexcept = delete;
        ~TransformedVertexBuffer(void) noexcept
        {
            glDeleteBuffers(1, &id);
        }

    public:
        [[nodiscard]]
        inline GLuint getId(void) const noexcept
        {
            return id;
        }

    private:
        GLuint id{ 0 };
    };

}
