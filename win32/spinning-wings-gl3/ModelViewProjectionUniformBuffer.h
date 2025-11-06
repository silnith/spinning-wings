#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include "Buffer.h"

namespace silnith::wings::gl3
{
    
    /// <summary>
    /// A class to reimplement the ModelView and Projection matrices that were
    /// removed from the OpenGL 3.2 Core profile.  The GLSL programs now take
    /// in uniforms instead.  The uniforms are provided in a buffer.
    /// </summary>
    /// <example>
    /// The GLSL declaration for this is:
    /// <code>
    /// uniform ModelViewProjection{
    ///     mat4 model;
    ///     mat4 view;
    ///     mat4 projection;
    /// };
    /// </code>
    /// </example>
    class ModelViewProjectionUniformBuffer : public Buffer
    {
    public:
        ModelViewProjectionUniformBuffer(void);
        ModelViewProjectionUniformBuffer(ModelViewProjectionUniformBuffer const&) = delete;
        ModelViewProjectionUniformBuffer& operator=(ModelViewProjectionUniformBuffer const&) = delete;
        ModelViewProjectionUniformBuffer(ModelViewProjectionUniformBuffer&&) noexcept = delete;
        ModelViewProjectionUniformBuffer& operator=(ModelViewProjectionUniformBuffer&&) noexcept = delete;
        virtual ~ModelViewProjectionUniformBuffer(void) noexcept override = default;
    };

}
