#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>
#include <string>

#include "Program.h"

#include "ModelViewProjectionUniformBuffer.h"

#include "WingGL3.h"

#include "WingGeometry.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// The GLSL program that renders all the wings.
    /// </summary>
    class WingRenderProgram : public Program
    {
    private:
        /// <summary>
        /// The GLSL vertex shader source code.  This applies the transformations
        /// that are relative to the position of the wing in the list of wings.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This has two input attributes, <c>vertex</c> and <c>color</c>.
        /// Vertex attributes are always extended to type <c>vec4</c> after being
        /// read from their source buffers.
        /// </para>
        /// <para>
        /// There is one varying output.
        /// <c>varyingColor</c> is of type <c>vec4</c>.
        /// </para>
        /// <para>
        /// There is one independent uniform.
        /// <c>deltaZ</c> is of type <c>vec2</c>.
        /// </para>
        /// <para>
        /// There is one uniform interface block that specifies the model, view,
        /// and projection matrices.
        /// </para>
        /// </remarks>
        static std::string const vertexShaderSourceCode;

        /// <summary>
        /// The GLSL fragment shader source code.  This simply sets the fragment
        /// color.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This has one input attribute.
        /// <c>varyingColor</c> is of type <c>vec4</c>.
        /// </para>
        /// <para>
        /// There is one varying output.
        /// <c>fragmentColor</c> is of type <c>vec4</c>.
        /// </para>
        /// </remarks>
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
        WingRenderProgram(void) = delete;
        WingRenderProgram(WingRenderProgram const&) = delete;
        WingRenderProgram& operator=(WingRenderProgram const&) = delete;
        WingRenderProgram(WingRenderProgram&&) noexcept = delete;
        WingRenderProgram& operator=(WingRenderProgram&&) noexcept = delete;
        virtual ~WingRenderProgram(void) noexcept override;

    public:
        explicit WingRenderProgram(std::shared_ptr<WingGeometry> const& wingGeometry);

        /// <summary>
        /// Renders the provided collection of wings.
        /// </summary>
        /// <param name="wings">The wings to render.</param>
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
        /// A pointer to the wing geometry object.
        /// </summary>
        std::shared_ptr<WingGeometry> wingGeometry{ nullptr };

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
        /// The vertex array object that stores all the rendering state for
		/// execution of the GLSL program.  This keeps the bound element array
		/// buffer and the vertex attribute arrays that are enabled.  The only
		/// things that need to be set are the specific buffers bound for each
        /// vertex attribute location, since those are different for every wing.
        /// </summary>
        /// <seealso cref="ArrayBuffer::UseForVertexAttribute"/>
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
