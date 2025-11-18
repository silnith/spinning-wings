#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl4
{

    Buffer::Buffer(void)
    {
        glGenBuffers(1, &name);
    }

    Buffer::~Buffer(void) noexcept
    {
        /*
         * The delete function silently ignores zero and any values that are not buffers.
         */
        glDeleteBuffers(1, &name);
    }

    GLuint Buffer::GetName(void) const
    {
        return name;
    }

}
