#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "WingTransformFeedback.h"

#include "ArrayBuffer.h"

namespace silnith::wings::gl4
{

    WingTransformFeedback::WingTransformFeedback(std::shared_ptr<ArrayBuffer const> vertexBuffer,
        std::shared_ptr<ArrayBuffer const> colorBuffer,
        std::shared_ptr<ArrayBuffer const> edgeColorBuffer)
        : vertexBuffer{ vertexBuffer },
        colorBuffer{ colorBuffer },
        edgeColorBuffer{ edgeColorBuffer }
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, GetName());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vertexBuffer->GetName());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, colorBuffer->GetName());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, edgeColorBuffer->GetName());
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    }

    std::shared_ptr<ArrayBuffer const> WingTransformFeedback::getVertexBuffer(void) const
    {
        return vertexBuffer;
    }

    std::shared_ptr<ArrayBuffer const> WingTransformFeedback::getColorBuffer(void) const
    {
        return colorBuffer;
    }

    std::shared_ptr<ArrayBuffer const> WingTransformFeedback::getEdgeColorBuffer(void) const
    {
        return edgeColorBuffer;
    }

}
