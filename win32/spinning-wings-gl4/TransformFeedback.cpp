#include <Windows.h>
#include <GL/glew.h>

#include "TransformFeedback.h"

namespace silnith::wings::gl4
{

    TransformFeedback::TransformFeedback(void)
        : name{ 0 }
    {
        glGenTransformFeedbacks(1, &name);
    }

    TransformFeedback::TransformFeedback(std::initializer_list<std::shared_ptr<ArrayBuffer const> > buffers)
        : TransformFeedback()
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, name);
        GLuint index{ 0 };
        for (std::shared_ptr<ArrayBuffer const> const& buffer : buffers)
        {
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index++, buffer->GetName());
        }
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    }

    TransformFeedback::~TransformFeedback(void)
    {
        glDeleteTransformFeedbacks(1, &name);
    }

    GLuint TransformFeedback::GetName(void) const
    {
        return name;
    }

}
