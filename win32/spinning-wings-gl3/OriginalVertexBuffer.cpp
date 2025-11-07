#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include "OriginalVertexBuffer.h"

#include "Buffer.h"

namespace silnith::wings::gl3
{

    OriginalVertexBuffer::OriginalVertexBuffer(void) : Buffer{}
    {
        std::array<GLfloat, numCoordinatesPerVertex * numVertices> constexpr quadVertices{
            1, 1,
            -1, 1,
            -1, -1,
            1, -1,
        };
        GLsizeiptr constexpr quadVerticesDataSize{ sizeof(GLfloat) * quadVertices.size() };
        static_assert(vertexCoordinateDataType == GL_FLOAT);

        glBindBuffer(GL_ARRAY_BUFFER, getId());
        glBufferData(GL_ARRAY_BUFFER, quadVerticesDataSize, quadVertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OriginalVertexBuffer::SetForVertexAttribute(GLuint vertexAttributeLocation) const
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
