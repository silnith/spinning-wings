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

    TransformFeedback::~TransformFeedback(void)
    {
        glDeleteTransformFeedbacks(1, &name);
    }

    GLuint TransformFeedback::GetName(void) const
    {
        return name;
    }

}
