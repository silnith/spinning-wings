#include <Windows.h>
#include <GL/glew.h>

#include <array>

#include "QuadRendererGL15.h"

namespace silnith::wings::gl
{

	QuadRendererGL15::QuadRendererGL15(void) : WingRenderer{}
	{
		GLsizei constexpr numVertices{ 4 };
		GLint constexpr numCoordinatesPerVertex{ 2 };
		std::array<GLfloat const, numCoordinatesPerVertex * numVertices> const wingVertices{
			1, 1,
			-1, 1,
			-1, -1,
			1, -1,
		};
		GLsizeiptr const wingVerticesDataSize{ sizeof(GLfloat) * wingVertices.size() };

		glGenBuffers(1, &wingBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, wingBufferObject);
		/*
		 * The vertex coordinates are defined in a local variable.  The array
		 * contents are read and transferred to graphics memory during the call to
		 * BufferData.  After that, the local array is no longer needed.
		 */
		glBufferData(GL_ARRAY_BUFFER, wingVerticesDataSize, wingVertices.data(), GL_STATIC_DRAW);
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 *
		 * If there is an ARRAY_BUFFER bound, then the last parameter is interpreted
		 * as an index/offset into the ARRAY_BUFFER, instead of a pointer.
		 */
		GLsizei constexpr wingVertexStride{ 0 };
		glVertexPointer(numCoordinatesPerVertex, GL_FLOAT, wingVertexStride, 0);

		/*
		 * Just like with the vertex coordinates, the indices are transferred
		 * to graphics memory when BufferData is called.  After that this array
		 * is no longer needed.
		 */
		std::array<GLuint const, numIndices> const wingIndices{
			0, 1, 2, 3,
		};
		GLsizeiptr const wingIndicesDataSize{ sizeof(GLuint) * numIndices };

		glGenBuffers(1, &wingIndicesBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, wingIndicesDataSize, wingIndices.data(), GL_STATIC_DRAW);

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
		 * ELEMENT_ARRAY_BUFFER, instead of a pointer.
		 *
		 * Vertices indexed by the ELEMENT_ARRAY_BUFFER are interpreted according to
		 * the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, numIndices, GL_UNSIGNED_INT, 0);
	}

}
