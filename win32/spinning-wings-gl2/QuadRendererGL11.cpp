#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include "QuadRendererGL11.h"

namespace silnith::wings::gl
{

	QuadRendererGL11::QuadRendererGL11(void) : QuadRenderer{},
		wingVertices{
			1, 1,
			-1, 1,
			-1, -1,
			1, -1,
		},
		wingIndices{ 0, 1, 2, 3, }
	{
		GLsizei constexpr wingVertexStride{ 0 };
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.  The format of the data is defined
		 * here, but the data is not read until the draw call is issued.
		 */
		glVertexPointer(numCoordinatesPerVertex, GL_FLOAT, wingVertexStride, wingVertices.data());

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
		 * This reads the specified number of indices from the provided client
		 * array.  For each index the corresponding vertex is read from the
		 * vertex array using the format specified in the VertexPointer call.
		 * The resulting vertices are assembled as though wrapped in Begin/End
		 * calls for the specified primitive type.
		 * 
		 * When compiling a display list, the dereferenced vertices are
		 * compiled into the display list.
		 */
		glDrawElements(GL_QUADS, numIndices, GL_UNSIGNED_INT, wingIndices.data());
	}

}
