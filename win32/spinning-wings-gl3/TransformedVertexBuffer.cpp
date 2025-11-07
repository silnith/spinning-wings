#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include "TransformedVertexBuffer.h"

#include "Buffer.h"

namespace silnith::wings::gl3
{

    TransformedVertexBuffer::TransformedVertexBuffer(void) : Buffer{}
    {
        // This static_assert is here as a reminder that the sizeof(GLfloat) must match.
        GLsizeiptr constexpr transformedVerticesDataSize{ sizeof(GLfloat) * numCoordinatesPerVertex * numVertices };
        static_assert(vertexCoordinateDataType == GL_FLOAT);

        glBindBuffer(GL_ARRAY_BUFFER, getId());
        glBufferData(GL_ARRAY_BUFFER, transformedVerticesDataSize, nullptr, GL_DYNAMIC_COPY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void TransformedVertexBuffer::SetForVertexAttribute(GLuint vertexAttributeLocation) const
    {
        glBindBuffer(GL_ARRAY_BUFFER, getId());
        glVertexAttribPointer(vertexAttributeLocation,
            numCoordinatesPerVertex,
            vertexCoordinateDataType,
            GL_FALSE,
            vertexStride,
            0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}
