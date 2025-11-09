#include <Windows.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <deque>
#include <memory>
#include <string>

#include "WingRenderProgram.h"

#include "Program.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "Shader.h"

#include "ArrayBuffer.h"
#include "ModelViewProjectionUniformBuffer.h"

#include "WingGeometry.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl3
{

    std::string const WingRenderProgram::vertexShaderSourceCode{ R"shaderText(#version 150

uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform vec2 deltaZ = vec2(15, 0.5);

in vec4 vertex;
in vec4 color;

smooth out vec4 varyingColor;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);

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
)shaderText" };

    std::string const WingRenderProgram::fragmentShaderSourceCode{ R"shaderText(#version 150

smooth in vec4 varyingColor;

out vec4 fragmentColor;

void main() {
    fragmentColor = varyingColor;
}
)shaderText" };

    WingRenderProgram::WingRenderProgram(std::shared_ptr<WingGeometry> const& wingGeometry) :
        Program{
            VertexShader{ vertexShaderSourceCode, Shader::rotateMatrixFunctionDefinition, Shader::translateMatrixFunctionDefinition },
            FragmentShader{ fragmentShaderSourceCode },
            "fragmentColor"s
        },
		wingGeometry{ wingGeometry },
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

		GLuint const programId{ getProgram() };
		/*
		 * Get the location (index) of the uniform block.
		 */
		GLuint const blockIndex{ glGetUniformBlockIndex(programId, "ModelViewProjection") };
		glUniformBlockBinding(programId, blockIndex, modelViewProjectionBindingIndex);

		/*
		 * Find the data size required by the uniform block.
		 */
		GLint modelViewProjectionUniformDataSize{ 0 };
		glGetActiveUniformBlockiv(programId, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &modelViewProjectionUniformDataSize);

		/*
		 * Find the locations (indices) of the components of the uniform block.
		 */
		GLsizei constexpr numUniforms{ 3 };
		std::array<GLchar const*, numUniforms> constexpr names{ "model", "view", "projection" };
		std::array<GLuint, numUniforms> uniformIndices{ 0, 0, 0 };
		glGetUniformIndices(programId, numUniforms, names.data(), uniformIndices.data());

		/*
		 * For each index in the uniform block, find the data offset for that index.
		 * This, finally, is the data offset we need to write to in order to set the
		 * data for that component.
		 */
		std::array<GLint, numUniforms> uniformOffsets{ 0, 0, 0, };
		glGetActiveUniformsiv(programId, numUniforms, uniformIndices.data(), GL_UNIFORM_OFFSET, uniformOffsets.data());
		static_assert(names[0] == "model");
		GLintptr const modelOffset{ uniformOffsets[0] };
		static_assert(names[1] == "view");
		GLintptr const viewOffset{ uniformOffsets[1] };
		static_assert(names[2] == "projection");
		GLintptr const projectionOffset{ uniformOffsets[2] };

		modelViewProjectionUniformBuffer = std::make_unique<ModelViewProjectionUniformBuffer>(modelViewProjectionUniformDataSize, modelOffset, viewOffset, projectionOffset);

#if !defined(NDEBUG)
		std::array<GLint, numUniforms> uniformTypes{ 0, 0, 0, };
		glGetActiveUniformsiv(programId, numUniforms, uniformIndices.data(), GL_UNIFORM_TYPE, uniformTypes.data());
		assert(uniformTypes[0] == GL_FLOAT_MAT4);
		assert(uniformTypes[1] == GL_FLOAT_MAT4);
		assert(uniformTypes[2] == GL_FLOAT_MAT4);

		/*
		 * The ModelViewProjectionUniformBuffer class will write the matrices as column-major.
		 */
		std::array<GLint, numUniforms> isRowMajor{ 0, 0, 0, };
		glGetActiveUniformsiv(programId, numUniforms, uniformIndices.data(), GL_UNIFORM_IS_ROW_MAJOR, isRowMajor.data());
		assert(isRowMajor[0] == 0);
		assert(isRowMajor[1] == 0);
		assert(isRowMajor[2] == 0);
#endif

		glBindBufferBase(GL_UNIFORM_BUFFER, modelViewProjectionBindingIndex, modelViewProjectionUniformBuffer->getId());
    }

    WingRenderProgram::~WingRenderProgram(void) noexcept
    {
        glDeleteVertexArrays(1, &vertexArray);
    }

	void WingRenderProgram::RenderWings(std::deque<Wing<GLfloat> > const& wings) const
	{
		useProgram();

		glBindVertexArray(vertexArray);

		/*
		 * First, draw the solid wings using their solid color.
		 */
		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (Wing<GLfloat> const& wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			glUniform2f(deltaZUniformLocation, deltaAngle, deltaZ);

			wing.getVertexBuffer()->UseForVertexAttribute(vertexAttributeLocation);

			wing.getColorBuffer()->UseForVertexAttribute(colorAttributeLocation);

			wingGeometry->RenderAsPolygons();
		}

		/*
		 * Second, draw the wing outlines using the outline color.
		 * The outlines have smoothing (antialiasing) enabled, which
		 * requires blending.
		 */
		deltaZ = 0;
		deltaAngle = 0;
		glEnable(GL_BLEND);
		for (Wing<GLfloat> const& wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			glUniform2f(deltaZUniformLocation, deltaAngle, deltaZ);

			wing.getVertexBuffer()->UseForVertexAttribute(vertexAttributeLocation);

			wing.getEdgeColorBuffer()->UseForVertexAttribute(colorAttributeLocation);

			wingGeometry->RenderAsOutline();
		}
		glDisable(GL_BLEND);

		glBindVertexArray(0);
	}

	void WingRenderProgram::Resize(GLfloat const width, GLfloat const height) const
	{
		modelViewProjectionUniformBuffer->SetProjectionMatrix(width, height);
	}

}
