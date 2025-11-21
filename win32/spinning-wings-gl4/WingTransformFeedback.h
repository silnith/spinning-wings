#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "TransformFeedback.h"

#include "ArrayBuffer.h"

namespace silnith::wings::gl4
{

    /// <summary>
    /// A class to encapsulate a transform feedback object and the array
    /// buffers that it writes into.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is tied to the specifics of the wing transform feedback program
    /// used to capture and preserve the wing-specific transformations.
    /// </para>
    /// <para>
    /// This requires OpenGL 4.0 or higher.
    /// </para>
    /// </remarks>
    class WingTransformFeedback : public TransformFeedback
    {
    public:
        /// <summary>
        /// Default constructor is deleted.  This object is meaningless without
        /// the buffers it writes into.
        /// </summary>
        WingTransformFeedback(void) = delete;

        /// <summary>
        /// Creates a transform feedback object and configures it to write into
        /// the provided vertex, color, and edge color buffers.
        /// </summary>
        /// <param name="vertexBuffer">The array buffer to write vertex coordinates into.</param>
        /// <param name="colorBuffer">The array buffer to write vertex colors into for the wing surface.</param>
        /// <param name="edgeColorBuffer">The array buffer to write vertex colors into for the wing edges.</param>
        explicit WingTransformFeedback(std::shared_ptr<ArrayBuffer const> vertexBuffer,
            std::shared_ptr<ArrayBuffer const> colorBuffer,
            std::shared_ptr<ArrayBuffer const> edgeColorBuffer);

#pragma region Rule of Five

    public:
        WingTransformFeedback(WingTransformFeedback const&) = delete;
        WingTransformFeedback& operator=(WingTransformFeedback const&) = delete;
        WingTransformFeedback(WingTransformFeedback&&) noexcept = delete;
        WingTransformFeedback& operator=(WingTransformFeedback&&) noexcept = delete;
        virtual ~WingTransformFeedback(void) noexcept override = default;

#pragma endregion

    public:
        /// <summary>
        /// Makes the vertex buffer the active buffer for the specified vertex
        /// attribute location.
        /// </summary>
        /// <param name="attributeLocation">The index of the generic vertex attribute.</param>
        void UseVertexBufferForVertexAttribute(GLuint attributeLocation) const;

        /// <summary>
        /// Makes the color buffer the active buffer for the specified vertex
        /// attribute location.
        /// </summary>
        /// <param name="attributeLocation">The index of the generic vertex attribute.</param>
        void UseColorBufferForVertexAttribute(GLuint attributeLocation) const;

        /// <summary>
        /// Makes the edge color buffer the active buffer for the specified vertex
        /// attribute location.
        /// </summary>
        /// <param name="attributeLocation">The index of the generic vertex attribute.</param>
        void UseEdgeColorBufferForVertexAttribute(GLuint attributeLocation) const;

    private:
        /// <summary>
        /// The array buffer that will receive vertex coordinates.
        /// </summary>
        std::shared_ptr<ArrayBuffer const> vertexBuffer{ nullptr };

        /// <summary>
        /// The array buffer that will receive color values.
        /// </summary>
        std::shared_ptr<ArrayBuffer const> colorBuffer{ nullptr };

        /// <summary>
        /// The array buffer that will receive edge color values.
        /// </summary>
        std::shared_ptr<ArrayBuffer const> edgeColorBuffer{ nullptr };
    };

}
