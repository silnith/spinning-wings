#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <span>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// A specialization of <see cref="Buffer"/> that is specifically for
    /// indices into vertex arrays.  This is intended to be bound as an
    /// <c>ELEMENT_ARRAY_BUFFER</c> and used for <c>DrawElements</c>.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The invariant established by this class is that the buffer is allocated
    /// and initialized with the provided list of indices.
    /// </para>
    /// </remarks>
    class ElementArrayBuffer : public Buffer
    {
#pragma region Rule of Five

    public:
        ElementArrayBuffer(ElementArrayBuffer const&) = delete;
        ElementArrayBuffer& operator=(ElementArrayBuffer const&) = delete;
        ElementArrayBuffer(ElementArrayBuffer&&) noexcept = delete;
        ElementArrayBuffer& operator=(ElementArrayBuffer&&) noexcept = delete;
        virtual ~ElementArrayBuffer(void) noexcept = default;

#pragma endregion

    public:
        /// <summary>
        /// Default constructor is deleted.  An element array is invalid without
        /// data to initialize it.
        /// </summary>
        ElementArrayBuffer(void) = delete;

        /// <summary>
        /// Allocates a buffer and initializes it to contain the provided data.
        /// </summary>
        /// <param name="indexData">The data to store in the buffer.</param>
        /// <param name="usageHint">A hint to the GL about how the data will be used.</param>
        explicit ElementArrayBuffer(std::span<GLuint const> indexData, GLenum usageHint);

        /// <summary>
        /// Makes this buffer the active buffer for <c>GL_ELEMENT_ARRAY_BUFFER</c>.
        /// </summary>
        /// <example>
        /// <code>
        /// ...;  // Bind the vertex attribute buffers.
        /// ElementArrayBuffer elementArrayBuffer{ ... };
        /// elementArrayBuffer.UseAsElementArray();
        /// glDrawElements(GL_TRIANGLE_FAN, elementArrayBuffer.getNumIndices(), elementArrayBuffer.getDataType(), elementArrayBuffer.getOffset());
        /// </code>
        /// </example>
        void UseAsElementArray() const;

        /// <summary>
        /// Returns the number of indices in the element array.
        /// </summary>
        /// <returns>The number of indices in the buffer.</returns>
        [[nodiscard]]
        GLsizei getNumIndices(void) const;

        /// <summary>
        /// Returns the data type of values in the element array.
        /// </summary>
        /// <returns>The data type of array values.</returns>
        [[nodiscard]]
        GLenum getDataType(void) const;

        /// <summary>
        /// Returns the offset of the first index in the element array.
        /// </summary>
        /// <returns>The offset to the first element.</returns>
        [[nodiscard]]
        GLvoid* getOffset(void) const;

    private:
        /// <summary>
        /// The number of indices in the buffer.
        /// </summary>
        GLsizei const numIndices{ 0 };

        /// <summary>
        /// The data type of values in the buffer.
        /// </summary>
        GLenum const dataType{ GL_UNSIGNED_INT };

        /// <summary>
        /// The byte offset of the first element in the buffer.
        /// </summary>
        GLvoid* const offset{ 0 };
    };

}
