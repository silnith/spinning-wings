#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <array>
#include <span>

#include "Buffer.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// A specialization of <see cref="Buffer"/> that is specifically for
    /// array data.  This means they will contain vertex data which will be
    /// used as inputs for vertex shaders.  Vertex coordinates and color values
    /// are common examples.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The invariant established by this class is that the buffer has memory
    /// allocated for a specific number of vertices with a specific number of
    /// components per vertex.
    /// </para>
    /// </remarks>
    class ArrayBuffer : public Buffer
    {
    public:
        ArrayBuffer(void) = delete;
        ArrayBuffer(ArrayBuffer const&) = delete;
        ArrayBuffer& operator=(ArrayBuffer const&) = delete;
        ArrayBuffer(ArrayBuffer&&) noexcept = delete;
        ArrayBuffer& operator=(ArrayBuffer&&) noexcept = delete;
        virtual ~ArrayBuffer(void) noexcept override = default;

    public:
        /// <summary>
        /// Constructs a buffer and initializes its contents with the provided
        /// data.
        /// </summary>
        /// <param name="numComponentsPerVertex">The number of components per vertex.</param>
        /// <param name="numVertices">The number of vertices for the buffer to hold.</param>
        /// <param name="data">The data to write into the buffer.</param>
        /// <param name="usageHint">A hint to the GL about how the data will be used.</param>
        explicit ArrayBuffer(GLint numComponentsPerVertex, GLsizei numVertices, std::span<GLfloat const> data, GLenum usageHint);

        /// <summary>
        /// Constructs a buffer and allocates sufficient space to hold the
        /// specified number of components of <c>GLfloat</c> for the specified
        /// number of vertices.  The actual contents will be empty.
        /// </summary>
        /// <param name="numComponentsPerVertex">The number of components per vertex.</param>
        /// <param name="numVertices">The number of vertices for the buffer to hold.</param>
        /// <param name="usageHint">A hint to the GL about how the data will be used.</param>
        explicit ArrayBuffer(GLint numComponentsPerVertex, GLsizei numVertices, GLenum usageHint);

        /// <summary>
        /// Makes this buffer the active buffer for the specified vertex
        /// attribute location.
        /// </summary>
        /// <param name="attributeLocation">The index of the generic vertex attribute.</param>
        void UseForVertexAttribute(GLuint attributeLocation) const;

        /// <summary>
        /// Returns the number of components per vertex that this buffer is
        /// sized to accommodate.
        /// </summary>
        /// <returns>The number of components that this buffer can hold for each vertex.</returns>
        [[nodiscard]]
        GLint getNumComponentsPerVertex(void) const;

        /// <summary>
        /// Returns the number of vertices that this buffer can hold.
        /// </summary>
        /// <returns>The number of vertices that this buffer can hold.</returns>
        [[nodiscard]]
        GLsizei getNumVertices(void) const;

    private:
        /// <summary>
        /// The number of components per vertex.
        /// </summary>
        GLint const numComponentsPerVertex{ 4 };

        /// <summary>
        /// The number of vertices that this buffer can hold.
        /// </summary>
        GLsizei const numVertices{ 0 };

        /// <summary>
        /// The data type of each component in the array.
        /// </summary>
        GLenum const dataType{ GL_FLOAT };

        /// <summary>
        /// Whether fixed-point data values should be normalized.
        /// </summary>
        GLboolean const normalized{ GL_FALSE };

        /// <summary>
        /// The byte offset between consecutive vertices.
        /// If <c>0</c>, the attribute values are tightly packed.
        /// </summary>
        GLsizei const stride{ 0 };

        /// <summary>
        /// The offset of the first vertex attribute in the array.
        /// </summary>
        GLvoid* const offset{ 0 };
    };

}
