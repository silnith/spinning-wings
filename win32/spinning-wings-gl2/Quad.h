#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <array>

namespace silnith::wings::gl
{

	/// <summary>
	/// The number of vertices in a quad.
	/// </summary>
	/// <seealso cref="quadVertices"/>
	static GLsizei constexpr numVertices{ 4 };

	/// <summary>
	/// The number of coordinates per vertex for <see cref="quadVertices"/>.
	/// </summary>
	static GLint constexpr numCoordinatesPerVertex{ 2 };

	/// <summary>
	/// The data type of each coordinate in <see cref="quadVertices"/>.
	/// </summary>
	static GLenum constexpr vertexCoordinateDataType{ GL_FLOAT };

	/// <summary>
	/// The byte offset between consecutive vertices in <see cref="quadVertices"/>.
	/// </summary>
	static GLsizei constexpr quadVertexStride{ 0 };

	/// <summary>
	/// The (x, y) vertex data for a single quad.
	/// </summary>
	/// <example>
	/// <code>
	/// glEnableClientState(GL_VERTEX_ARRAY);
	/// glVertexPointer(numCoordinatesPerVertex, vertexCoordinateDataType, quadVertexStride, quadVertices.data());
	/// </code>
	/// </example>
	/// <seealso cref="numVertices"/>
	/// <seealso cref="numCoordinatesPerVertex"/>
	/// <seealso cref="vertexCoordinateDataType"/>
	/// <seealso cref="quadVertexStride"/>
	/// <seealso cref="quadIndices"/>
	static std::array<GLfloat, numCoordinatesPerVertex * numVertices> constexpr quadVertices{
		1, 1,
		-1, 1,
		-1, -1,
		1, -1,
	};

	/// <summary>
	/// The size in bytes of the <see cref="quadVertices"/> data.
	/// </summary>
	/// <seealso cref="quadVertices"/>
	static GLsizeiptr constexpr quadVerticesDataSize{ sizeof(GLfloat) * quadVertices.size() };

	/// <summary>
	/// The number of indices in a quad.
	/// </summary>
	/// <seealso cref="quadIndices"/>
	static GLsizei constexpr numIndices{ 4 };

	/// <summary>
	/// The data type of each index in <see cref="quadIndices"/>.
	/// </summary>
	/// <seealso cref="quadIndices"/>
	static GLenum constexpr quadIndexDataType{ GL_UNSIGNED_INT };

	/// <summary>
	/// The indices into <see cref="quadVertices"/> for rendering a single quad.
	/// </summary>
	/// <example>
	/// <code>
	/// glEnableClientState(GL_VERTEX_ARRAY);
	/// glVertexPointer(numCoordinatesPerVertex, vertexCoordinateDataType, quadVertexStride, quadVertices.data());
	/// 
	/// glDrawElements(GL_QUADS, numIndices, quadIndexDataType, quadIndices.data());
	/// </code>
	/// </example>
	/// <seealso cref="numIndices"/>
	/// <seealso cref="quadIndexDataType"/>
	/// <seealso cref="quadVertices"/>
	static std::array<GLuint, numIndices> constexpr quadIndices{
		0, 1, 2, 3,
	};

	/// <summary>
	/// The size in bytes of the <see cref="quadIndices"/> data.
	/// </summary>
	/// <seealso cref="quadIndices"/>
	static GLsizeiptr constexpr quadIndicesDataSize{ sizeof(GLuint) * quadIndices.size() };

	// TODO: add static_asserts that all indices are within the vertex data

}
