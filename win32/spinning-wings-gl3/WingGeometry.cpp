#include <Windows.h>
#include <GL/glew.h>

#include "WingGeometry.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// The number of vertices in a wing.
    /// </summary>
    static GLsizei constexpr numVertices{ 4 };

    /// <summary>
    /// The number of indices in a wing.
    /// </summary>
    /// <seealso cref="quadIndices"/>
    static GLsizei constexpr numIndices{ 4 };

    /// <summary>
    /// The number of coordinates per vertex in the original source data.
    /// </summary>
    static GLint constexpr numCoordinatesPerVertex{ 2 };

    WingGeometry::WingGeometry(void) :
        vertexArrayBuffer{
            numCoordinatesPerVertex,
            numVertices,
            std::array<GLfloat const, numCoordinatesPerVertex * numVertices>{
                1, 1,
                -1, 1,
                -1, -1,
                1, -1,
            },
            GL_STATIC_DRAW,
        },
        elementArrayBuffer{
            std::array<GLuint const, numIndices>{
                0, 1, 2, 3,
            },
            GL_STATIC_DRAW,
        }
    {
        // TODO: Find a way to static_assert that all indices point to valid vertices.
    }

    GLsizei WingGeometry::getNumVertices(void) const
    {
        return vertexArrayBuffer.getNumVertices();
    }

    void WingGeometry::UseForVertexAttribute(GLuint attributeLocation) const
    {
        vertexArrayBuffer.UseForVertexAttribute(attributeLocation);
    }

    void WingGeometry::UseElementArrayBuffer(void) const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer.getId());
    }

    void WingGeometry::RenderAsPoints(void) const
    {
        glDrawArrays(GL_POINTS, 0, vertexArrayBuffer.getNumVertices());
    }

    void WingGeometry::RenderAsPolygons(void) const
    {
        glDrawElements(GL_TRIANGLE_FAN, elementArrayBuffer.getNumIndices(), elementArrayBuffer.getDataType(), elementArrayBuffer.getOffset());
    }

    void WingGeometry::RenderAsOutline(void) const
    {
        glDrawElements(GL_LINE_LOOP, elementArrayBuffer.getNumIndices(), elementArrayBuffer.getDataType(), elementArrayBuffer.getOffset());
    }

}
