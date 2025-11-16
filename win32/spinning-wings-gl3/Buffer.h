#pragma once

#include <Windows.h>
#include <GL/glew.h>

namespace silnith::wings::gl3
{

    /// <summary>
    /// A class to manage an OpenGL buffer object.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The invariant established by this class is only that the buffer has a
    /// valid OpenGL identifier.  No actual memory is allocated for the buffer.
    /// For that, use one of the subclasses.
    /// </para>
    /// <para>
    /// Various utility classes designed to represent special-use buffers can
    /// use this as a base to handle the RAII part.
    /// </para>
    /// </remarks>
    class Buffer
    {
    public:
        /// <summary>
        /// Constructs a new buffer object.  The buffer has no attached storage
        /// until some is allocated using <c>glBufferData</c>.
        /// </summary>
        explicit Buffer(void);
        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;
        Buffer(Buffer&&) noexcept = delete;
        Buffer& operator=(Buffer&&) noexcept = delete;
        /// <summary>
        /// Deletes the buffer object.
        /// </summary>
        virtual ~Buffer(void) noexcept;

    public:
        /// <summary>
        /// Returns the buffer ID.
        /// </summary>
        /// <returns>The buffer ID.</returns>
        [[nodiscard]]
        GLuint getId(void) const;

    private:
        /// <summary>
        /// The OpenGL identifier for the buffer.
        /// </summary>
        GLuint id{ 0 };
    };

}
