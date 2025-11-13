#include <Windows.h>
#include <GL/glew.h>

#include <array>
#include <initializer_list>
#include <map>
#include <string>
#include <vector>

#include <cassert>

#include "WingTransformProgram.h"

#include "VertexShader.h"

#include "ArrayBuffer.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl3
{

    /// <summary>
    /// The number of vertex coordinates captured from the transform
    /// feedback program.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The vertex shader sets the built-in variable <c>gl_Position</c>,
    /// which is of type <c>vec4</c>.
    /// </para>
    /// </remarks>
    static GLint constexpr numCapturedCoordinatesPerVertex{ 4 };

    /// <summary>
    /// The number of color components captured from the transform
    /// feedback program.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The vertex shader sets the varying variables <c>varyingWingColor</c>
    /// and <c>varyingEdgeColor</c>, which are both of type <c>vec3</c>.
    /// </para>
    /// </remarks>
    static GLint constexpr numCapturedColorComponentsPerVertex{ 3 };

    GLuint constexpr glPositionBindingPoint{ 0 };
    GLuint constexpr varyingWingColorBindingPoint{ 1 };
    GLuint constexpr varyingEdgeColorBindingPoint{ 2 };
    constexpr char const* capturedVaryingZero{ "gl_Position" };
    constexpr char const* capturedVaryingOne{ "varyingWingColor" };
    constexpr char const* capturedVaryingTwo{ "varyingEdgeColor" };
    // TODO: Find a way to static_assert these match the initializer list passed to the superclass constructor.

    WingTransformProgram::WingTransformProgram(std::shared_ptr<WingGeometry> const& wingGeometry,
        std::shared_ptr<VertexShader const> const& rotateMatrixShader,
        std::shared_ptr<VertexShader const> const& translateMatrixShader)
        : Program{
            std::initializer_list<std::shared_ptr<VertexShader const> >{
                std::make_shared<VertexShader const>(std::initializer_list<std::string>{
                    Shader::versionDeclaration,
                    R"shaderText(
uniform vec2 radiusAngle;
uniform vec3 rollPitchYaw;
uniform vec3 color;
uniform vec3 edgeColor = vec3(1, 1, 1);

/*
 * Vertex attributes are always extended to type vec4 by the GL when read.
 * Any missing values are filled in from the default values (0, 0, 0, 1).
 * This is true regardless of whatever semantic meaning the programmer may
 * assign to the specific attribute.
 */
in vec4 vertex;

smooth out vec3 varyingWingColor;
smooth out vec3 varyingEdgeColor;

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);
)shaderText",
                    Shader::rotateMatrixFunctionDeclaration,
                    Shader::translateMatrixFunctionDeclaration,
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
                capturedVaryingZero,
                capturedVaryingOne,
                capturedVaryingTwo,
            }
        },
        wingGeometry{ wingGeometry },
        radiusAngleUniformLocation{ getUniformLocation("radiusAngle"s) },
        rollPitchYawUniformLocation{ getUniformLocation("rollPitchYaw"s) },
        colorUniformLocation{ getUniformLocation("color"s) },
        edgeColorUniformLocation{ getUniformLocation("edgeColor"s) }
    {
        GLuint const vertexAttributeLocation{ getAttributeLocation("vertex"s) };
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glEnableVertexAttribArray(vertexAttributeLocation);
        wingGeometry->UseForVertexAttribute(vertexAttributeLocation);
        glBindVertexArray(0);
    }

    WingTransformProgram::~WingTransformProgram(void) noexcept
    {
        glDeleteVertexArrays(1, &vertexArray);
    }

    std::shared_ptr<ArrayBuffer> WingTransformProgram::CreateVertexBuffer() const
    {
        return std::make_shared<ArrayBuffer>(numCapturedCoordinatesPerVertex, wingGeometry->getNumVertices(), GL_DYNAMIC_COPY);
    }

    std::shared_ptr<ArrayBuffer> WingTransformProgram::CreateColorBuffer() const
    {
        return std::make_shared<ArrayBuffer>(numCapturedColorComponentsPerVertex, wingGeometry->getNumVertices(), GL_DYNAMIC_COPY);
    }

    void WingTransformProgram::TransformWing(GLfloat radius, GLfloat angle,
        GLfloat roll, GLfloat pitch, GLfloat yaw,
        GLfloat red, GLfloat green, GLfloat blue,
        ArrayBuffer const& vertexBuffer,
        ArrayBuffer const& colorBuffer,
        ArrayBuffer const& edgeColorBuffer) const
    {
        useProgram();

        glUniform2f(radiusAngleUniformLocation, radius, angle);
        glUniform3f(rollPitchYawUniformLocation, roll, pitch, yaw);
        glUniform3f(colorUniformLocation, red, green, blue);
        glUniform3f(edgeColorUniformLocation, 1, 1, 1);

        glBindVertexArray(vertexArray);

        // Assert that the buffers are expecting the correct data layout.
        assert(vertexBuffer.getNumComponentsPerVertex() == 4);
        assert(colorBuffer.getNumComponentsPerVertex() == 3);
        assert(edgeColorBuffer.getNumComponentsPerVertex() == 3);
        // Assert that the buffers are sized appropriately.
        assert(vertexBuffer.getNumVertices() == wingGeometry->getNumVertices());
        assert(colorBuffer.getNumVertices() == wingGeometry->getNumVertices());
        assert(edgeColorBuffer.getNumVertices() == wingGeometry->getNumVertices());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, glPositionBindingPoint, vertexBuffer.getId());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, varyingWingColorBindingPoint, colorBuffer.getId());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, varyingEdgeColorBindingPoint, edgeColorBuffer.getId());

        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
        wingGeometry->RenderAsPoints();
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);

        glBindVertexArray(0);

        glUseProgram(0);
    }

}
