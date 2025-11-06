#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include "IndexDataBuffer.h"

#include "Buffer.h"

namespace silnith::wings::gl3
{

    IndexDataBuffer::IndexDataBuffer(void) : Buffer{}
    {
        std::array<GLuint, numIndices> constexpr quadIndices{
            0, 1, 2, 3,
        };
        GLsizeiptr constexpr quadIndicesDataSize{ sizeof(GLuint) * quadIndices.size() };
        static_assert(quadIndexDataType == GL_UNSIGNED_INT);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getId());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesDataSize, quadIndices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}
