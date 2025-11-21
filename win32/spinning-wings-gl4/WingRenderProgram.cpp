#include <Windows.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <deque>
#include <initializer_list>
#include <memory>
#include <string>

#include "WingRenderProgram.h"

#include "Program.h"

#include "WingGeometry.h"
#include "WingGL4.h"
#include "WingTransformFeedback.h"

#include "VertexShader.h"
#include "FragmentShader.h"
#include "Shader.h"
#include "ModelViewProjectionUniformBuffer.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl4
{

    WingRenderProgram::WingRenderProgram(std::shared_ptr<WingGeometry const> wingGeometry,
        std::shared_ptr<VertexShader const> rotateMatrixShader,
        std::shared_ptr<VertexShader const> translateMatrixShader)
        : Program{
            std::initializer_list<std::shared_ptr<VertexShader const> >{
                std::make_shared<VertexShader const>(std::initializer_list<std::string>{
                    Shader::versionDeclaration,
                    ModelViewProjectionUniformBuffer::uniformBlockDeclaration,
                    R"shaderText(
uniform vec2 deltaZ = vec2(15, 0.5);

in vec4 vertex;
in vec4 color;

smooth out vec4 varyingColor;

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);
)shaderText",
                    Shader::rotateMatrixFunctionDeclaration,
                    Shader::translateMatrixFunctionDeclaration,
                    R"shaderText(
void main() {
    float deltaAngle = deltaZ[0];
    float dZ = deltaZ[1];

    mat4 modelViewProjection = projection * view * model;

    varyingColor = color;
    gl_Position = modelViewProjection
                  * translate(vec3(0, 0, dZ))
                  * rotate(deltaAngle, zAxis)
                  * vertex;
}
)shaderText",
                }),
                rotateMatrixShader,
                translateMatrixShader,
            },
            std::initializer_list<std::shared_ptr<FragmentShader const> >{
                std::make_shared<FragmentShader const>(std::initializer_list<std::string>{
                    Shader::versionDeclaration,
                    R"shaderText(
smooth in vec4 varyingColor;

out vec4 fragmentColor;

void main() {
    fragmentColor = varyingColor;
}
)shaderText",
                }),
            },
            "fragmentColor"
        },
        wingGeometry{ wingGeometry },
        modelViewProjectionUniformBuffer{ nullptr },
        vertexArray{ 0 },
        deltaZUniformLocation{ getUniformLocation("deltaZ"s) },
        vertexAttributeLocation{ getAttributeLocation("vertex"s) },
        colorAttributeLocation{ getAttributeLocation("color"s) }
    {
        glGenVertexArrays(1, &vertexArray);

        glBindVertexArray(vertexArray);
        glEnableVertexAttribArray(vertexAttributeLocation);
        glEnableVertexAttribArray(colorAttributeLocation);
        wingGeometry->UseElementArrayBuffer();
        glBindVertexArray(0);

        modelViewProjectionUniformBuffer = ModelViewProjectionUniformBuffer::MakeBuffer(GetName(), modelViewProjectionBindingIndex);

        /*
         * Set up the initial camera position.
         */
        glm::mat4 const view2{ glm::lookAt(
            glm::vec3{ 0, 50, 50 },
            glm::vec3{ 0, 0, 13 },
            glm::vec3{ 0, 0, 1 }) };

        std::array<GLfloat, 4 * 4> const view{
            view2[0][0],
            view2[0][1],
            view2[0][2],
            view2[0][3],

            view2[1][0],
            view2[1][1],
            view2[1][2],
            view2[1][3],

            view2[2][0],
            view2[2][1],
            view2[2][2],
            view2[2][3],

            view2[3][0],
            view2[3][1],
            view2[3][2],
            view2[3][3],
        };

        modelViewProjectionUniformBuffer->SetViewMatrix(view);
    }

    WingRenderProgram::~WingRenderProgram(void) noexcept
    {
        glDeleteVertexArrays(1, &vertexArray);
    }

    void WingRenderProgram::RenderWings(std::deque<Wing> const& wings) const
    {
        useProgram();

        glBindVertexArray(vertexArray);

        GLfloat deltaZ{ 0 };
        GLfloat deltaAngle{ 0 };
        for (Wing const& wing : wings) {
            deltaZ += wing.getDeltaZ();
            deltaAngle += wing.getDeltaAngle();

            glUniform2f(deltaZUniformLocation, deltaAngle, deltaZ);

            std::shared_ptr<WingTransformFeedback const> const wingTransformFeedbackObject{ wing.getTransformFeedbackObject() };

            wingTransformFeedbackObject->UseVertexBufferForVertexAttribute(vertexAttributeLocation);

            wingTransformFeedbackObject->UseColorBufferForVertexAttribute(colorAttributeLocation);

            wingGeometry->RenderAsPolygons();
        }

        deltaZ = 0;
        deltaAngle = 0;
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        for (Wing const& wing : wings) {
            deltaZ += wing.getDeltaZ();
            deltaAngle += wing.getDeltaAngle();

            glUniform2f(deltaZUniformLocation, deltaAngle, deltaZ);

            std::shared_ptr<WingTransformFeedback const> const wingTransformFeedbackObject{ wing.getTransformFeedbackObject() };

            wingTransformFeedbackObject->UseVertexBufferForVertexAttribute(vertexAttributeLocation);

            wingTransformFeedbackObject->UseEdgeColorBufferForVertexAttribute(colorAttributeLocation);

            wingGeometry->RenderAsOutline();
        }
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);

        glUseProgram(0);
    }

    void WingRenderProgram::Ortho(GLfloat const width, GLfloat const height) const
    {
        /*
         * These multipliers account for the aspect ratio of the window, so that
         * the rendering does not distort.  The conditional is so that the larger
         * number is always divided by the smaller, resulting in a multiplier no
         * less than one.  This way, the viewing area is always expanded rather than
         * contracted, and the expected viewing frustum is never clipped.
         */
        GLfloat xmult{ 1.0 };
        GLfloat ymult{ 1.0 };
        if (width > height)
        {
            xmult = width / height;
        }
        else
        {
            ymult = height / width;
        }

        /*
         * The view frustum was hand-selected to match the parameters to the
         * curve generators and the initial camera position.
         */
        GLfloat constexpr defaultLeft{ -20 };
        GLfloat constexpr defaultRight{ 20 };
        GLfloat constexpr defaultBottom{ -20 };
        GLfloat constexpr defaultTop{ 20 };
        GLfloat constexpr defaultNear{ 35 };
        GLfloat constexpr defaultFar{ 105 };

        GLfloat const left{ defaultLeft * xmult };
        GLfloat const right{ defaultRight * xmult };
        GLfloat const bottom{ defaultBottom * ymult };
        GLfloat const top{ defaultTop * ymult };
        GLfloat const nearZ{ defaultNear };
        GLfloat const farZ{ defaultFar };

        GLfloat const viewWidth{ right - left };
        GLfloat const viewHeight{ top - bottom };
        GLfloat const viewDepth{ farZ - nearZ };

        assert(viewWidth > 0);
        assert(viewHeight > 0);
        assert(viewDepth > 0);

        /*
         * Set up the projection matrix.
         * The projection matrix is only used for the viewing frustum.
         * Things like camera position belong in the modelview matrix.
         */
        std::array<GLfloat, 4 * 4> const projection{
            // column 0
            static_cast<GLfloat>(2) / viewWidth,
            0,
            0,
            0,

            // column 1
            0,
            static_cast<GLfloat>(2) / viewHeight,
            0,
            0,

            // column 2
            0,
            0,
            static_cast<GLfloat>(-2) / viewDepth,
            0,

            // column 3
            -(right + left) / viewWidth,
            -(top + bottom) / viewHeight,
            -(farZ + nearZ) / viewDepth,
            static_cast<GLfloat>(1),
        };

        modelViewProjectionUniformBuffer->SetProjectionMatrix(projection);
    }

}
