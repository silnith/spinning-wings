#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>

#include "Program.h"

#include "WingGeometry.h"
#include "ModelViewProjectionUniformBuffer.h"
#include "WingGL4.h"
#include "RotateVertexShader.h"
#include "TranslateVertexShader.h"
#include "VertexShader.h"

namespace silnith::wings::gl4
{

    /// <summary>
    /// The GLSL program that renders all the wings.
    /// </summary>
    class WingRenderProgram : public Program
    {
    public:
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
        /// <summary>
        /// Default constructor is deleted.  The render program requires the
        /// wing geometry and utility shaders that are shared between multiple
        /// programs.
        /// </summary>
        WingRenderProgram(void) = delete;

        explicit WingRenderProgram(std::shared_ptr<WingGeometry const> wingGeometry,
            std::shared_ptr<RotateVertexShader const> rotateMatrixShader,
            std::shared_ptr<TranslateVertexShader const> translateMatrixShader);

#pragma region Rule of Five

        WingRenderProgram(WingRenderProgram const&) = delete;
        WingRenderProgram& operator=(WingRenderProgram const&) = delete;
        WingRenderProgram(WingRenderProgram&&) noexcept = delete;
        WingRenderProgram& operator=(WingRenderProgram&&) noexcept = delete;
        virtual ~WingRenderProgram(void) noexcept override;

#pragma endregion

    public:
        /// <summary>
        /// Renders the provided collection of wings.
        /// </summary>
        /// <param name="wings">The wings to render.</param>
        void RenderWings(std::deque<Wing> const& wings) const;

        /// <summary>
        /// Sets up the orthographic projection that transforms modelview coordinates
        /// into normalized device coordinates.
        /// This takes into account the aspect ratio of the viewport.
        /// </summary>
        /// <param name="width">The viewport width.</param>
        /// <param name="height">The viewport height.</param>
        void Ortho(GLfloat const width, GLfloat const height) const;

    private:
        /// <summary>
        /// A pointer to the wing geometry object.
        /// </summary>
        std::shared_ptr<WingGeometry const> wingGeometry{ nullptr };

        /// <summary>
        /// The uniform buffer for the ModelViewProjection matrices.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This is a pointer because significant OpenGL queries must be issued
        /// before all of the required parameters are known.
        /// </para>
        /// </remarks>
        std::shared_ptr<ModelViewProjectionUniformBuffer const> modelViewProjectionUniformBuffer{ nullptr };

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
        GLuint vertexAttributeLocation{ 0 };

        /// <summary>
        /// The location of the vertex attribute <c>color</c>.
        /// </summary>
        GLuint colorAttributeLocation{ 0 };
    };

}
