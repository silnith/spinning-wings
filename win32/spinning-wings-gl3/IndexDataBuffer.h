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
    /// the indices into <see cref="OriginalVertexBuffer"/> for a wing.
    /// </summary>
    class IndexDataBuffer
    {
    public:
        /// <summary>
        /// The number of indices in a quad.
        /// </summary>
        /// <seealso cref="quadIndices"/>
        static GLsizei constexpr numIndices{ 4 };

        /// <summary>
        /// The data type of each index.
        /// </summary>
        /// <seealso cref="quadIndices"/>
        static GLenum constexpr quadIndexDataType{ GL_UNSIGNED_INT };

    public:
        IndexDataBuffer(void)
        {
            glGenBuffers(1, &id);

            std::array<GLuint, numIndices> constexpr quadIndices{
                0, 1, 2, 3,
            };
            GLsizeiptr constexpr quadIndicesDataSize{ sizeof(GLuint) * quadIndices.size() };

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesDataSize, quadIndices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        IndexDataBuffer(IndexDataBuffer const&) = delete;
        IndexDataBuffer& operator=(IndexDataBuffer const&) = delete;
        IndexDataBuffer(IndexDataBuffer&&) noexcept = delete;
        IndexDataBuffer& operator=(IndexDataBuffer&&) noexcept = delete;
        ~IndexDataBuffer(void)
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
