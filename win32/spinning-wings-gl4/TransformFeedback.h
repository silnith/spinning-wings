#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <memory>

#include "ArrayBuffer.h"

namespace silnith::wings::gl4
{

    class TransformFeedback
    {
    public:
        explicit TransformFeedback(void);

        explicit TransformFeedback(std::initializer_list<std::shared_ptr<ArrayBuffer const> > buffers);

    public:
        TransformFeedback(TransformFeedback const&) = delete;
        TransformFeedback& operator=(TransformFeedback const&) = delete;
        TransformFeedback(TransformFeedback&&) noexcept = delete;
        TransformFeedback& operator=(TransformFeedback&&) noexcept = delete;
        ~TransformFeedback(void) noexcept;

    public:
        /// <summary>
        /// foo
        /// </summary>
        /// <returns></returns>
        [[nodiscard]]
        GLuint GetName(void) const;

    private:
        GLuint name{ 0 };
    };

}
