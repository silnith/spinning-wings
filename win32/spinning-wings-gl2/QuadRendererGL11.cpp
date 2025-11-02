#include <Windows.h>
#include <GL/glew.h>

#include "QuadRendererGL11.h"

#include "Quad.h"

namespace silnith::wings::gl
{

	QuadRendererGL11::QuadRendererGL11(void)
	{
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 */
		glVertexPointer(numCoordinatesPerVertex, vertexCoordinateDataType, quadVertexStride, quadVertices.data());

		/*
		 * EnableClientState is executed immediately, it is not compiled into
		 * a display list.
		 */
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	QuadRendererGL11::~QuadRendererGL11(void) noexcept
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	
	void QuadRendererGL11::DrawQuad(void) const
	{
		/*
		 * When compiling a display list,
		 * DrawElements dereferences vertex pointers according to the client
		 * state, and the dereferenced vertices are compiled into the
		 * display list.
		 */
		glDrawElements(GL_QUADS, numIndices, quadIndexDataType, quadIndices.data());
	}

}
