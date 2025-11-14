#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    Buffer::Buffer(void)
    {
        glGenBuffers(1, &id);
    }

    Buffer::~Buffer(void) noexcept
    {
        /*
         * The delete function silently ignores zero and any values that are not buffers.
         */
        glDeleteBuffers(1, &id);
    }

    GLuint Buffer::getId(void) const
    {
        return id;
    }

}
