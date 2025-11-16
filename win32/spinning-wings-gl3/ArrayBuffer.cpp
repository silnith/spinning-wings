#include <Windows.h>
#include <GL/glew.h>

#include <span>

#include <cassert>
#include <cstddef>

#include "ArrayBuffer.h"

namespace silnith::wings::gl3
{

    ArrayBuffer::ArrayBuffer(GLint numComponentsPerVertex, GLsizei numVertices, std::span<GLfloat const> data, GLenum usageHint)
        : Buffer{},
        numComponentsPerVertex{ numComponentsPerVertex },
        numVertices{ numVertices },
        dataType{ GL_FLOAT },
        normalized{ GL_FALSE },
        stride{ 0 },
        offset{ 0 }
    {
        // Confirm that the parameters match each other.
        assert(data.size() == static_cast<std::size_t>(numComponentsPerVertex) * static_cast<std::size_t>(numVertices));
        GLsizeiptr const dataSize{ static_cast<GLsizeiptr>(data.size_bytes()) };

        glBindBuffer(GL_ARRAY_BUFFER, GetName());
        glBufferData(GL_ARRAY_BUFFER, dataSize, data.data(), usageHint);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ArrayBuffer::ArrayBuffer(GLint numComponentsPerVertex, GLsizei numVertices, GLenum usageHint)
        : Buffer{},
        numComponentsPerVertex{ numComponentsPerVertex },
        numVertices{ numVertices },
        dataType{ GL_FLOAT },
        normalized{ GL_FALSE },
        stride{ 0 },
        offset{ 0 }
    {
        GLsizeiptr const dataSize{ static_cast<GLsizeiptr>(sizeof(GLfloat) * numComponentsPerVertex * numVertices) };

        glBindBuffer(GL_ARRAY_BUFFER, GetName());
        glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, usageHint);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void ArrayBuffer::UseForVertexAttribute(GLuint attributeLocation) const
    {
        /*
         * The currently-bound array buffer is captured as part of the state
         * associated with a vertex attribute pointer.  Therefore, after the
         * call to glVertexAttribPointer, the buffer can be unbound without
         * affecting the stored state of the vertex attribute pointer.
         */
        glBindBuffer(GL_ARRAY_BUFFER, GetName());
        glVertexAttribPointer(attributeLocation, numComponentsPerVertex, dataType, normalized, stride, offset);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLint ArrayBuffer::getNumComponentsPerVertex(void) const
    {
        return numComponentsPerVertex;
    }

    GLsizei ArrayBuffer::getNumVertices(void) const
    {
        return numVertices;
    }

}
