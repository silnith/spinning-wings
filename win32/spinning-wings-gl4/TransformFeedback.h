#pragma once

#include <Windows.h>
#include <GL/glew.h>

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

#pragma region Rule of Five

    public:
        TransformFeedback(TransformFeedback const&) = delete;
        TransformFeedback& operator=(TransformFeedback const&) = delete;
        TransformFeedback(TransformFeedback&&) noexcept = delete;
        TransformFeedback& operator=(TransformFeedback&&) noexcept = delete;
        virtual ~TransformFeedback(void) noexcept;

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
