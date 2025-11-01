#include <Windows.h>
#include <GL/glew.h>

#include "QuadRendererGL15.h"

#include "Quad.h"

namespace silnith::wings::gl
{

	QuadRendererGL15::QuadRendererGL15(void)
	{
		glGenBuffers(1, &wingBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, wingBufferObject);
		glBufferData(GL_ARRAY_BUFFER, quadVerticesDataSize, quadVertices.data(), GL_STATIC_DRAW);
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 *
		 * If there is an ARRAY_BUFFER bound, then the last parameter is interpreted
		 * as an index/offset into the ARRAY_BUFFER.
		 */
		glVertexPointer(numCoordinatesPerVertex, vertexCoordinateDataType, quadVertexStride, 0);

		glGenBuffers(1, &wingIndicesBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesDataSize, quadIndices.data(), GL_STATIC_DRAW);

		glEnableClientState(GL_VERTEX_ARRAY);
	}

	QuadRendererGL15::~QuadRendererGL15(void) noexcept
	{
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &wingIndicesBufferObject);
		glDeleteBuffers(1, &wingBufferObject);
	}

	void QuadRendererGL15::DrawQuad(void) const
	{
		/*
		 * When compiled into a display list,
		 * DrawElements dereferences vertex pointers according to the client
		 * state, and the dereferenced vertices are compiled into the
		 * display list.
		 *
		 * If there is an ELEMENT_ARRAY_BUFFER bound, then the last parameter
		 * to DrawElements is interpreted as an index/offset into the
		 * ELEMENT_ARRAY_BUFFER.
		 *
		 * Vertices indexed by the ELEMENT_ARRAY_BUFFER are interpreted according to
		 * the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, numIndices, quadIndexDataType, 0);
	}

}
