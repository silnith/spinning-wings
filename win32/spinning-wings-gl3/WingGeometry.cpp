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

    std::shared_ptr<ArrayBuffer> WingGeometry::CreateBuffer(GLint numComponentsPerVertex) const
    {
        return std::make_shared<ArrayBuffer>(numComponentsPerVertex, getNumVertices(), GL_DYNAMIC_COPY);
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
        elementArrayBuffer.UseAsElementArray();
    }

    void WingGeometry::RenderAsPoints(void) const
    {
        /*
         * Since this ignores any connection between vertices, there is no need
         * to go through the indirection of the element array (which is there
         * to allow vertex reuse).
         */
        glDrawArrays(GL_POINTS, 0, vertexArrayBuffer.getNumVertices());
    }

    void WingGeometry::RenderAsPolygons(void) const
    {
        /*
         * OpenGL 3.2 Core removed the quad primitive type.
         * Triangle fan is the closest approximation.
         */
        glDrawElements(GL_TRIANGLE_FAN, elementArrayBuffer.getNumIndices(), elementArrayBuffer.getDataType(), elementArrayBuffer.getOffset());
    }

    void WingGeometry::RenderAsOutline(void) const
    {
        /*
         * Since OpenGL 3.2 Core does not support the quad primitive type,
         * we can no longer render as polygons using line mode.
         */
        glDrawElements(GL_LINE_LOOP, elementArrayBuffer.getNumIndices(), elementArrayBuffer.getDataType(), elementArrayBuffer.getOffset());
    }

}
