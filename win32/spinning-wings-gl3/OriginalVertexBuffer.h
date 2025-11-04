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
    /// the original untransformed vertices for a wing.
    /// </summary>
    class OriginalVertexBuffer
    {
    public:
        /// <summary>
        /// The number of vertices in a single wing.
        /// </summary>
        static GLsizei constexpr numVertices{ 4 };

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
        OriginalVertexBuffer(void)
        {
            glGenBuffers(1, &id);

            std::array<GLfloat, numCoordinatesPerVertex * numVertices> constexpr quadVertices{
                1, 1,
                -1, 1,
                -1, -1,
                1, -1,
            };
            GLsizeiptr constexpr quadVerticesDataSize{ sizeof(GLfloat) * quadVertices.size() };

            glBindBuffer(GL_ARRAY_BUFFER, id);
            glBufferData(GL_ARRAY_BUFFER, quadVerticesDataSize, quadVertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        OriginalVertexBuffer(OriginalVertexBuffer const&) = delete;
        OriginalVertexBuffer& operator=(OriginalVertexBuffer const&) = delete;
        OriginalVertexBuffer(OriginalVertexBuffer&&) noexcept = delete;
        OriginalVertexBuffer& operator=(OriginalVertexBuffer&&) noexcept = delete;
        ~OriginalVertexBuffer(void) noexcept
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
