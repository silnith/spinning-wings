#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <memory>

#include "ArrayBuffer.h"

namespace silnith::wings::gl4
{

    /// <summary>
    /// A class to represent the transform feedback object.
    /// This is a convenience tool to aggregate all of the parameters required
    /// for transform feedback, and store them in the GL memory.  Using this,
    /// when a new wing reuses the buffers of a previous wing, the transform
    /// feedback parameters do not need to be specified again.
    /// </summary>
    class TransformFeedback
    {
    public:
        /// <summary>
        /// Constructs a transform feedback object.  Initially no buffers
        /// will be specified in it.
        /// </summary>
        explicit TransformFeedback(void);

        /// <summary>
        /// Constructs a transform feedback object that writes output into the
        /// provided list of output buffers, in order.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The list of buffers must match the captured varying variables
        /// specified when the <see cref="Program"/> object is created.
        /// </para>
        /// </remarks>
        /// <param name="buffers">The array buffers into which transform feedback output will be written.</param>
        explicit TransformFeedback(std::initializer_list<std::shared_ptr<ArrayBuffer const> > buffers);

#pragma region Rule of Five

    public:
        TransformFeedback(TransformFeedback const&) = delete;
        TransformFeedback& operator=(TransformFeedback const&) = delete;
        TransformFeedback(TransformFeedback&&) noexcept = delete;
        TransformFeedback& operator=(TransformFeedback&&) noexcept = delete;
        ~TransformFeedback(void) noexcept;

#pragma endregion

    public:
        /// <summary>
        /// Returns the OpenGL name for the transform feedback object.
        /// </summary>
        /// <returns>The transform feedback object name.</returns>
        [[nodiscard]]
        GLuint GetName(void) const;

    private:
        /// <summary>
        /// The OpenGL name for the transform feedback object.
        /// </summary>
        GLuint name{ 0 };
    };

}
