#include <Windows.h>
#include <GL/glew.h>

#include <span>

#include "ElementArrayBuffer.h"

#include "Buffer.h"

namespace silnith::wings::gl4
{

    ElementArrayBuffer::ElementArrayBuffer(std::span<GLuint const> indexData, GLenum usageHint)
        : Buffer{},
        numIndices{ static_cast<GLsizei>(indexData.size()) },
        dataType{ GL_UNSIGNED_INT },
        offset{ 0 }
    {
        GLsizeiptr const dataSize{ static_cast<GLsizeiptr>(indexData.size_bytes()) };

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetName());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, indexData.data(), usageHint);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void ElementArrayBuffer::UseAsElementArray() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetName());
    }

    GLsizei ElementArrayBuffer::getNumIndices(void) const
    {
        return numIndices;
    }

    GLenum ElementArrayBuffer::getDataType(void) const
    {
        return dataType;
    }

    GLvoid* ElementArrayBuffer::getOffset(void) const
    {
        return offset;
    }

}
