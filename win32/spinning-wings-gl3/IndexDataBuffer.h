#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// A class to encapsulate a GL buffer object as well as the critical
    /// parameters that define the data stored in it.  This object contains
    /// the indices into <see cref="OriginalVertexBuffer"/> for a wing.
    /// </summary>
    class IndexDataBuffer : public Buffer
    {
    public:
        /// <summary>
        /// The number of indices in a wing.
        /// </summary>
        /// <seealso cref="quadIndices"/>
        static GLsizei constexpr numIndices{ 4 };

        /// <summary>
        /// The data type of each index.
        /// </summary>
        /// <seealso cref="quadIndices"/>
        static GLenum constexpr quadIndexDataType{ GL_UNSIGNED_INT };

    public:
        /// <summary>
        /// Constructs the element array buffer and populates it with the indices for rendering a wing.
        /// </summary>
        IndexDataBuffer(void);
        IndexDataBuffer(IndexDataBuffer const&) = delete;
        IndexDataBuffer& operator=(IndexDataBuffer const&) = delete;
        IndexDataBuffer(IndexDataBuffer&&) noexcept = delete;
        IndexDataBuffer& operator=(IndexDataBuffer&&) noexcept = delete;
        virtual ~IndexDataBuffer(void) noexcept override = default;
    };

}
