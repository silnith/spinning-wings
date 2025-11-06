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
        Buffer(void);
        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;
        Buffer(Buffer&&) noexcept = delete;
        Buffer& operator=(Buffer&&) noexcept = delete;
        virtual ~Buffer(void) noexcept;

    public:
        /// <summary>
        /// Returns the buffer ID.
        /// </summary>
        /// <returns>The buffer ID.</returns>
        [[nodiscard]]
        inline GLuint getId(void) const noexcept
        {
            return id;
        }

    private:
        /// <summary>
        /// The OpenGL identifier for the buffer.
        /// </summary>
        GLuint id{ 0 };
    };

}
