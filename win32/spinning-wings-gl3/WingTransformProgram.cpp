#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

#include "WingTransformProgram.h"

#include "VertexShader.h"

#include "OriginalVertexBuffer.h"
#include "TransformedVertexBuffer.h"
#include "TransformedColorBuffer.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl3
{

    /// <summary>
    /// The GLSL vertex shader.  This applies the wing-specific transformations,
    /// and then captures the transformed vertices into a buffer for later use.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This has one input attribute named <c>vertex</c> of type <c>vec4</c>.
    /// </para>
    /// <para>
    /// There are three varying outputs.
    /// <c>gl_Position</c> is of type <c>vec4</c>.
    /// <c>varyingWingColor</c> is of type <c>vec3</c>.
    /// <c>varyingEdgeColor</c> is of type <c>vec3</c>.
    /// </para>
    /// <para>
    /// There are four uniforms.
    /// <c>radiusAngle</c> is of type <c>vec2</c>.
    /// <c>rollPitchYaw</c> is of type <c>vec3</c>.
    /// <c>color</c> is of type <c>vec3</c>.
    /// <c>edgeColor</c> is of type <c>vec3</c>, and is optional.
    /// </para>
    /// </remarks>
    std::string const WingTransformProgram::sourceCode{ R"shaderText(#version 150

uniform vec2 radiusAngle;
uniform vec3 rollPitchYaw;
uniform vec3 color;
uniform vec3 edgeColor = vec3(1, 1, 1);

in vec4 vertex;

smooth out vec3 varyingWingColor;
smooth out vec3 varyingEdgeColor;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);

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
)shaderText" };

    GLuint constexpr glPositionBindingPoint{ 0 };
    GLuint constexpr varyingWingColorBindingPoint{ 1 };
    GLuint constexpr varyingEdgeColorBindingPoint{ 2 };
    constexpr char const * capturedVaryingZero{ "gl_Position" };
    constexpr char const* capturedVaryingOne{ "varyingWingColor" };
    constexpr char const* capturedVaryingTwo{ "varyingEdgeColor" };
    // TODO: Find a way to static_assert these match the initializer list passed to the superclass constructor.

    WingTransformProgram::WingTransformProgram(void) :
        Program{
            VertexShader{ sourceCode, Shader::rotateMatrixFunctionDefinition, Shader::translateMatrixFunctionDefinition },
            { capturedVaryingZero, capturedVaryingOne, capturedVaryingTwo }
        },
        radiusAngleUniformLocation{ getUniformLocation("radiusAngle"s) },
        rollPitchYawUniformLocation{ getUniformLocation("rollPitchYaw"s) },
        colorUniformLocation{ getUniformLocation("color"s) },
        edgeColorUniformLocation{ getUniformLocation("edgeColor"s) }
    {
        GLuint const vertexAttributeLocation{ getAttributeLocation("vertex"s) };
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glEnableVertexAttribArray(vertexAttributeLocation);
        glBindBuffer(GL_ARRAY_BUFFER, originalVertices.getId());
        glVertexAttribPointer(vertexAttributeLocation,
            OriginalVertexBuffer::numCoordinatesPerVertex,
            OriginalVertexBuffer::vertexCoordinateDataType,
            GL_FALSE,
            OriginalVertexBuffer::vertexStride,
            0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    WingTransformProgram::~WingTransformProgram(void) noexcept
    {
        glDeleteVertexArrays(1, &vertexArray);
    }

    void WingTransformProgram::Render(GLfloat radius, GLfloat angle,
        GLfloat roll, GLfloat pitch, GLfloat yaw,
        GLfloat red, GLfloat green, GLfloat blue,
        TransformedVertexBuffer const& vertexBuffer,
        TransformedColorBuffer const& colorBuffer,
        TransformedColorBuffer const& edgeColorBuffer) const
    {
        useProgram();
        glUniform2f(radiusAngleUniformLocation, radius, angle);
        glUniform3f(rollPitchYawUniformLocation, roll, pitch, yaw);
        glUniform3f(colorUniformLocation, red, green, blue);
        glUniform3f(edgeColorUniformLocation, 1, 1, 1);

        glBindVertexArray(vertexArray);

        static_assert(TransformedVertexBuffer::numVertices == OriginalVertexBuffer::numVertices);
        static_assert(TransformedColorBuffer::numVertices == OriginalVertexBuffer::numVertices);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, glPositionBindingPoint, vertexBuffer.getId());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, varyingWingColorBindingPoint, colorBuffer.getId());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, varyingEdgeColorBindingPoint, edgeColorBuffer.getId());

        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, OriginalVertexBuffer::numVertices);
        glEndTransformFeedback();

        glBindVertexArray(0);
    }

}
