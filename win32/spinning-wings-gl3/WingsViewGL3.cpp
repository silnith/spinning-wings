#include "WingsViewGL3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cassert>
#include <deque>
#include <memory>
#include <sstream>

#include "CurveGenerator.h"
#include "WingGL3.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

namespace silnith::wings::gl3
{

	typedef std::deque<Wing<GLuint, GLfloat> > wing_list;

	size_t constexpr numWings{ 40 };

	GLint glMajorVersion{ 1 };
	GLint glMinorVersion{ 0 };

	wing_list wings{};

	CurveGenerator<GLfloat> radiusCurve{ 10.0f, -15.0f, 15.0f, false, 0.1f, 0.01f, 150 };
	CurveGenerator<GLfloat> angleCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 2.0f, 0.05f, 120) };
	CurveGenerator<GLfloat> deltaAngleCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(15.0f, 0.2f, 0.02f, 80) };
	CurveGenerator<GLfloat> deltaZCurve{ 0.5f, 0.4f, 0.7f, false, 0.01f, 0.001f, 200 };
	CurveGenerator<GLfloat> rollCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 1.0f, 0.25f, 80) };
	CurveGenerator<GLfloat> pitchCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 2.0f, 0.25f, 40) };
	CurveGenerator<GLfloat> yawCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 1.5f, 0.25f, 50) };
	CurveGenerator<GLfloat> redCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 95) };
	CurveGenerator<GLfloat> greenCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 40) };
	CurveGenerator<GLfloat> blueCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 70) };

	std::unique_ptr<Program> wingTransformProgram{ nullptr };
	std::unique_ptr<Program> renderProgram{ nullptr };

	GLsizei constexpr numVertices{ 4 };
	GLsizei constexpr numIndices{ 4 };
	/// <summary>
	/// The initial untransformed vertices for a single quad.
	/// After binding, enable using <c>glVertexAttribPointer(..., 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0)</c>.
	/// </summary>
	GLuint originalVertexBuffer{ 0 };
	/// <summary>
	/// The indices into <c>originalVertexBuffer</c>.
	/// </summary>
	GLuint wingIndexBuffer{ 0 };
	/// <summary>
	/// The vertex array used for transform feedback.
	/// This maintains the state of the enabled vertex attributes.
	/// </summary>
	GLuint wingTransformVertexArray{ 0 };
	/// <summary>
	/// The vertex array used for rendering.
	/// This maintains the state of the enabled vertex attributes,
	/// as well as the binding for the ELEMENT_ARRAY_BUFFER.
	/// </summary>
	GLuint renderVertexArray{ 0 };

	/// <summary>
	/// The uniform buffer for the ModelViewProjection matrices.
	/// </summary>
	GLuint modelViewProjectionUniformBuffer{ 0 };
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
	GLuint constexpr modelViewProjectionBindingIndex{ 0 };

	/// <summary>
	/// The offset into the uniform buffer where the model matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr modelOffset{ 0 };
	/// <summary>
	/// The offset into the uniform buffer where the view matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr viewOffset{ 0 };
	/// <summary>
	/// The offset into the uniform buffer where the projection matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr projectionOffset{ 0 };

	void InitializeOpenGLState(void)
	{
		glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

		glEnable(GL_DEPTH_TEST);

		glPolygonOffset(0.5, 2);
		glEnable(GL_POLYGON_OFFSET_FILL);

		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glm::mat4 const view2{ glm::lookAt(
			glm::vec3{ 0, 50, 50 },
			glm::vec3{ 0, 0, 13 },
			glm::vec3{ 0, 0, 1 }) };

		{
			std::array<GLfloat, 2 * numVertices> constexpr quadVertices{
				1, 1,
				-1, 1,
				-1, -1,
				1, -1,
			};
			GLsizeiptr constexpr quadVerticesDataSize{ sizeof(GLfloat) * quadVertices.size() };

			glGenBuffers(1, &originalVertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, quadVerticesDataSize, quadVertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{
			std::array<GLuint, numIndices> constexpr quadIndices{
				0, 1, 2, 3,
			};
			GLsizeiptr constexpr quadIndicesDataSize{ sizeof(GLuint) * quadIndices.size() };
			// TODO: static_assert that each index is less than numVertices

			glGenBuffers(1, &wingIndexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesDataSize, quadIndices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		std::string const rotateMatrixFunctionDeclaration{
			R"shaderText(
mat4 rotate(in float angle, in vec3 axis) {
    // OpenGL has always specified angles in degrees.
    // Trigonometric functions operate on radians.
    float c = cos(radians(angle));
    float s = sin(radians(angle));

    mat3 initial = outerProduct(axis, axis)
                   * (1 - c);
    mat3 c_part = mat3(c);
    mat3 s_part = mat3(0, axis.z, -axis.y,
                       -axis.z, 0, axis.x,
                       axis.y, -axis.x, 0)
                  * s;
    mat3 temp = initial + c_part + s_part;

    mat4 rotation = mat4(1.0);
    rotation[0].xyz = temp[0];
    rotation[1].xyz = temp[1];
    rotation[2].xyz = temp[2];

    return rotation;
}
)shaderText"
		};
		std::string const translateMatrixFunctionDeclaration{
			R"shaderText(
mat4 translate(in vec3 move) {
    mat4 trans = mat4(1.0);
    trans[3].xyz = move;
    return trans;
}
)shaderText"
		};
		std::string const scaleMatrixFunctionDeclaration{
			R"shaderText(
mat4 scale(in vec3 factor) {
    return mat4(vec4(factor, 1));
}
)shaderText"
		};

		wingTransformProgram.reset(new Program{
			VertexShader{
				R"shaderText(#version 150

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
)shaderText",
				rotateMatrixFunctionDeclaration,
				translateMatrixFunctionDeclaration,
			},
			{
				"gl_Position",
				"varyingWingColor",
				"varyingEdgeColor",
			}
		});
		glGenVertexArrays(1, &wingTransformVertexArray);
		glBindVertexArray(wingTransformVertexArray);
		glEnableVertexAttribArray(wingTransformProgram->getAttributeLocation("vertex"));
		glBindVertexArray(0);

		renderProgram.reset(new Program{
			VertexShader{
				R"shaderText(#version 150

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
    float deltaZ = deltaZ[1];

    mat4 modelViewProjection = projection * view * model;

    varyingColor = color;
    gl_Position = modelViewProjection
                  * translate(vec3(0, 0, deltaZ))
                  * rotate(deltaAngle, zAxis)
                  * vertex;
}
)shaderText",
				rotateMatrixFunctionDeclaration,
				translateMatrixFunctionDeclaration,
			},
			FragmentShader{
				R"shaderText(#version 150

smooth in vec4 varyingColor;

out vec4 fragmentColor;

void main() {
    fragmentColor = varyingColor;
}
)shaderText",
			},
			"fragmentColor"
		});
		glGenVertexArrays(1, &renderVertexArray);
		glBindVertexArray(renderVertexArray);
		glEnableVertexAttribArray(renderProgram->getAttributeLocation("vertex"));
		glEnableVertexAttribArray(renderProgram->getAttributeLocation("color"));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);
		glBindVertexArray(0);

		GLsizei dataSize{ 0 };
		{
			GLuint const programId{ renderProgram->getProgram() };
			GLuint const blockIndex{ glGetUniformBlockIndex(programId, "ModelViewProjection")};
			glUniformBlockBinding(programId, blockIndex, modelViewProjectionBindingIndex);

			{
				std::array<GLchar const*, 3> constexpr names{ "model", "view", "projection" };
				std::array<GLuint, names.size()> uniformIndices{ 0, 0, 0 };
				static_assert(names.size() == uniformIndices.size());
				glGetUniformIndices(programId, names.size(), names.data(), uniformIndices.data());

				std::array<GLint, uniformIndices.size()> uniformOffsets{ 0, 0, 0, };
				glGetActiveUniformsiv(programId, uniformIndices.size(), uniformIndices.data(), GL_UNIFORM_OFFSET, uniformOffsets.data());
				modelOffset = uniformOffsets[0];
				viewOffset = uniformOffsets[1];
				projectionOffset = uniformOffsets[2];
			}

			{
				GLint modelViewProjectionUniformDataSize{ 0 };
				glGetActiveUniformBlockiv(programId, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &modelViewProjectionUniformDataSize);
				dataSize = modelViewProjectionUniformDataSize;
			}
		}

		glGenBuffers(1, &modelViewProjectionUniformBuffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, modelViewProjectionBindingIndex, modelViewProjectionUniformBuffer);

		/*
		 * The C++ syntax makes this look like it is row-major, but OpenGL will read it as column-major.
		 * However, that is irrelevant because the identity matrix is its own transposition.
		 */
		std::array<GLfloat, 16> constexpr identity{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};
		GLsizeiptr constexpr identityDataSize{ sizeof(GLfloat) * identity.size() };

		std::array<GLfloat, 16> const view{
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
		GLsizeiptr constexpr viewDataSize{ sizeof(GLfloat) * view.size() };

		glBindBuffer(GL_UNIFORM_BUFFER, modelViewProjectionUniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, modelOffset, identityDataSize, identity.data());
		glBufferSubData(GL_UNIFORM_BUFFER, viewOffset, viewDataSize, view.data());
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, identityDataSize, identity.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void CleanupOpenGLState(void)
	{
		for (wing_list::const_reference wing : wings)
		{
			GLuint const vertexBuffer{ wing.getVertexBuffer() };
			GLuint const colorBuffer{ wing.getColorBuffer() };
			GLuint const edgeColorBuffer{ wing.getEdgeColorBuffer() };
			glDeleteBuffers(1, &vertexBuffer);
			glDeleteBuffers(1, &colorBuffer);
			glDeleteBuffers(1, &edgeColorBuffer);
		}

		glDeleteBuffers(1, &modelViewProjectionUniformBuffer);

		glDeleteVertexArrays(1, &renderVertexArray);
		glDeleteVertexArrays(1, &wingTransformVertexArray);

		glDeleteBuffers(1, &wingIndexBuffer);
		glDeleteBuffers(1, &originalVertexBuffer);

		wingTransformProgram.reset(nullptr);
		renderProgram.reset(nullptr);
	}

	void AdvanceAnimation(void)
	{
		GLfloat const radius{ radiusCurve.getNextValue() };
		GLfloat const angle{ angleCurve.getNextValue() };
		GLfloat const deltaAngle{ deltaAngleCurve.getNextValue() };
		GLfloat const deltaZ{ deltaZCurve.getNextValue() };
		GLfloat const roll{ rollCurve.getNextValue() };
		GLfloat const pitch{ pitchCurve.getNextValue() };
		GLfloat const yaw{ yawCurve.getNextValue() };
		GLfloat const red{ redCurve.getNextValue() };
		GLfloat const green{ greenCurve.getNextValue() };
		GLfloat const blue{ blueCurve.getNextValue() };

		GLuint wingVertexBuffer{ 0 };
		GLuint wingColorBuffer{ 0 };
		GLuint wingEdgeColorBuffer{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			glGenBuffers(1, &wingVertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, wingVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * numVertices, nullptr, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &wingColorBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, wingColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, nullptr, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &wingEdgeColorBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, wingEdgeColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, nullptr, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else
		{
			// This block is simply so lastWing goes out of scope before the pop_back.
			{
				wing_list::const_reference lastWing{ wings.back() };
				wingVertexBuffer = lastWing.getVertexBuffer();
				wingColorBuffer = lastWing.getColorBuffer();
				wingEdgeColorBuffer = lastWing.getEdgeColorBuffer();
			}
			wings.pop_back();
		}

		wings.emplace_front(wingVertexBuffer,
			wingColorBuffer,
			wingEdgeColorBuffer,
			deltaAngle, deltaZ);

		wingTransformProgram->useProgram();
		glUniform2f(wingTransformProgram->getUniformLocation("radiusAngle"), radius, angle);
		glUniform3f(wingTransformProgram->getUniformLocation("rollPitchYaw"), roll, pitch, yaw);
		glUniform3f(wingTransformProgram->getUniformLocation("color"), red, green, blue);
		//glUniform3f(wingTransformProgram->getUniformLocation("edgeColor"), 1, 1, 1);

		glBindVertexArray(wingTransformVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
		glVertexAttribPointer(wingTransformProgram->getAttributeLocation("vertex"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, wingVertexBuffer);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, wingColorBuffer);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, wingEdgeColorBuffer);

		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, numVertices);
		glEndTransformFeedback();

		glBindVertexArray(0);
	}

	void DrawFrame(void)
	{
		renderProgram->useProgram();
		GLuint const vertexAttribLocation{ renderProgram->getAttributeLocation("vertex") };
		GLuint const colorAttribLocation{ renderProgram->getAttributeLocation("color") };

		glBindVertexArray(renderVertexArray);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (wing_list::const_reference wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			glUniform2f(renderProgram->getUniformLocation("deltaZ"), deltaAngle, deltaZ);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getVertexBuffer());
			glVertexAttribPointer(vertexAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getEdgeColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElements(GL_LINE_LOOP, numIndices, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElements(GL_TRIANGLE_FAN, numIndices, GL_UNSIGNED_INT, 0);
		}

		glFlush();

		glBindVertexArray(0);
	}

	void Ortho(GLfloat const width, GLfloat const height)
	{
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

		std::array<GLfloat, 16> const projection{
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
		GLsizeiptr constexpr projectionDataSize{ sizeof(GLfloat) * projection.size() };

		glBindBuffer(GL_UNIFORM_BUFFER, modelViewProjectionUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, projectionDataSize, projection.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void Resize(GLsizei width, GLsizei height)
	{
		glViewport(0, 0, width, height);

		Ortho(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	}

}
