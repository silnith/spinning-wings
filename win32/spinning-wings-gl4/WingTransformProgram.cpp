#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <memory>
#include <string>

#include "WingTransformProgram.h"

#include "WingGeometry.h"
#include "WingTransformFeedback.h"
#include "RotateVertexShader.h"
#include "TranslateVertexShader.h"
#include "VertexShader.h"
#include "Program.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl4
{

    WingTransformProgram::WingTransformProgram(std::shared_ptr<WingGeometry const> wingGeometry,
        std::shared_ptr<RotateVertexShader const> rotateMatrixShader,
        std::shared_ptr<TranslateVertexShader const> translateMatrixShader)
        : Program{
            std::initializer_list<std::shared_ptr<VertexShader const> >{
                std::make_shared<VertexShader const>(std::initializer_list<std::string>{
                    Shader::versionDeclaration,
                    R"shaderText(
uniform vec2 radiusAngle;
uniform vec3 rollPitchYaw;
uniform vec3 color;
uniform vec3 edgeColor = vec3(1, 1, 1);

in vec4 vertex;

smooth out vec3 varyingWingColor;
smooth out vec3 varyingEdgeColor;

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);
)shaderText",
                    rotateMatrixShader->GetDeclaration(),
                    translateMatrixShader->GetDeclaration(),
                    R"shaderText(
void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    varyingWingColor = color;
    varyingEdgeColor = edgeColor;

    mat4 wingTransformation = rotate(angle, zAxis)
                              * translate(vec3(radius, 0, 0))
                              * rotate(-yaw, zAxis)
                              * rotate(-pitch, yAxis)
                              * rotate(roll, xAxis);
    gl_Position = wingTransformation * vertex;
}
)shaderText",
                }),
                rotateMatrixShader,
                translateMatrixShader,
            },
            std::initializer_list<std::string>{
                "gl_Position"s,
                "varyingWingColor"s,
                "varyingEdgeColor"s,
            }
        },
        wingGeometry{ wingGeometry },
        vertexArray{ 0 },
        radiusAngleUniformLocation{ getUniformLocation("radiusAngle"s) },
        rollPitchYawUniformLocation{ getUniformLocation("rollPitchYaw"s) },
        colorUniformLocation{ getUniformLocation("color"s) },
        edgeColorUniformLocation{ getUniformLocation("edgeColor"s) }
    {
        glGenVertexArrays(1, &vertexArray);

        GLuint const vertexAttributeLocation{ getAttributeLocation("vertex"s) };
        glBindVertexArray(vertexArray);
        glEnableVertexAttribArray(vertexAttributeLocation);
        wingGeometry->UseForVertexAttribute(vertexAttributeLocation);
        glBindVertexArray(0);
    }

    WingTransformProgram::~WingTransformProgram(void) noexcept
    {
        glDeleteVertexArrays(1, &vertexArray);
    }

    std::shared_ptr<WingTransformFeedback const> WingTransformProgram::CreateTransformFeedback(void) const
    {
        std::shared_ptr<ArrayBuffer const> const wingVertexBuffer{ wingGeometry->CreateBuffer(4) };
        std::shared_ptr<ArrayBuffer const> const wingColorBuffer{ wingGeometry->CreateBuffer(3) };
        std::shared_ptr<ArrayBuffer const> const wingEdgeColorBuffer{ wingGeometry->CreateBuffer(3) };

        return std::make_shared<WingTransformFeedback const>(
            wingVertexBuffer,
            wingColorBuffer,
            wingEdgeColorBuffer);
    }

    void WingTransformProgram::TransformWing(GLfloat radius, GLfloat angle,
        GLfloat roll, GLfloat pitch, GLfloat yaw,
        GLfloat red, GLfloat green, GLfloat blue,
        WingTransformFeedback const& wingTransformFeedbackObject) const
    {
        useProgram();

        glUniform2f(radiusAngleUniformLocation, radius, angle);
        glUniform3f(rollPitchYawUniformLocation, roll, pitch, yaw);
        glUniform3f(colorUniformLocation, red, green, blue);
        glUniform3f(edgeColorUniformLocation, 1, 1, 1);

        glBindVertexArray(vertexArray);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, wingTransformFeedbackObject.GetName());

        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
        wingGeometry->RenderAsPoints();
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        glBindVertexArray(0);

        glUseProgram(0);
    }

}
