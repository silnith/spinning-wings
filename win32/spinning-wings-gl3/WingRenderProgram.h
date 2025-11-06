#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>
#include <string>

#include "IndexDataBuffer.h"

#include "Program.h"

#include "VertexShader.h"
#include "FragmentShader.h"

#include "ModelViewProjectionUniformBuffer.h"

#include "WingGL3.h"

namespace silnith::wings::gl3
{

    class WingRenderProgram : public Program
    {
    private:
        static std::string const vertexShaderSourceCode;

        static std::string const fragmentShaderSourceCode;

        /// <summary>
        /// The uniform buffer binding index for the ModelViewProjection matrices.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Uniform buffers use an indirection mechanism.  Rather than binding them directly,
        /// a shader program specifies an index into a set of binding points.
        /// The application can then update the uniform buffer bound to that index
        /// independently of whichever shader program is active.
        /// </para>
        /// <para>
        /// In this case, there is only one shader program and one uniform buffer.
        /// So this simply uses the first index in the array of binding points.
        /// </para>
        /// </remarks>
        static GLuint constexpr modelViewProjectionBindingIndex{ 0 };

    public:
        WingRenderProgram(void);
        WingRenderProgram(WingRenderProgram const&) = delete;
        WingRenderProgram& operator=(WingRenderProgram const&) = delete;
        WingRenderProgram(WingRenderProgram&&) noexcept = delete;
        WingRenderProgram& operator=(WingRenderProgram&&) noexcept = delete;
        virtual ~WingRenderProgram(void) noexcept override;

    public:
        void RenderWings(std::deque<Wing<GLfloat> > const& wings) const;

        /// <summary>
        /// Sets up the orthographic projection that transforms modelview coordinates
        /// into normalized device coordinates.
        /// This takes into account the aspect ratio of the viewport.
        /// </summary>
        /// <param name="width">The viewport width.</param>
        /// <param name="height">The viewport height.</param>
        void Resize(GLfloat const width, GLfloat const height) const;

    private:
        /// <summary>
        /// The indices into <c>OriginalVertexBuffer</c>.
        /// </summary>
        IndexDataBuffer const indexBuffer{};

        /// <summary>
        /// The uniform buffer for the ModelViewProjection matrices.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This is a pointer because significant OpenGL queries must be issued
        /// before all of the required parameters are known.
        /// </para>
        /// </remarks>
        std::unique_ptr<ModelViewProjectionUniformBuffer> modelViewProjectionUniformBuffer{ nullptr };

        /// <summary>
        /// The vertex array used for rendering.
        /// This maintains the state of the enabled vertex attributes,
        /// as well as the binding for the ELEMENT_ARRAY_BUFFER.
        /// </summary>
        GLuint vertexArray{ 0 };

        /// <summary>
        /// The location of the uniform variable <c>deltaZ</c>.
        /// </summary>
        /// <seealso cref="glUniform2f"/>
        GLint deltaZUniformLocation{ 0 };

        /// <summary>
        /// The location of the vertex attribute <c>vertex</c>.
        /// </summary>
        /// <seealso cref="TransformedVertexBuffer"/>
        GLuint vertexAttributeLocation{ 0 };
        
        /// <summary>
        /// The location of the vertex attribute <c>color</c>.
        /// </summary>
        /// <seealso cref="TransformedColorBuffer"/>
        GLuint colorAttributeLocation{ 0 };
    };

}
