#include "WingsViewGL3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "WingGL3.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

namespace silnith::wings::gl3
{

	typedef std::deque<Wing<GLuint, GLfloat> > wing_list;

	size_t const numWings{ 40 };

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

	Program* wingTransformProgram{ nullptr };
	Program* renderProgram{ nullptr };

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
	GLuint const modelViewProjectionBindingIndex{ 0 };

	/// <summary>
	/// The model matrix to be passed to the vertex shaders.
	/// </summary>
	/// <remarks>
	/// <para>
	/// Remember that this is used in column-major order,
	/// but in the source code it looks like it is in row-major order.
	/// Therefore you need to transpose it in your mind.
	/// Fortunately all the hard-coded source code appearances
	/// are purely the identity matrix, where transposition does nothing.
	/// </para>
	/// </remarks>
	GLfloat model[16]{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	/// <summary>
	/// The offset into the uniform buffer where the model matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr modelOffset{ 0 };
	GLsizeiptr const modelSize{ sizeof(model) };
	static_assert(modelSize == sizeof(GLfloat) * 4 * 4, "Check whether the uniform buffer allocation is correct.");
	/// <summary>
	/// The view matrix to be passed to the vertex shaders.
	/// </summary>
	/// <remarks>
	/// <para>
	/// Remember that this is used in column-major order,
	/// but in the source code it looks like it is in row-major order.
	/// Therefore you need to transpose it in your mind.
	/// Fortunately all the hard-coded source code appearances
	/// are purely the identity matrix, where transposition does nothing.
	/// </para>
	/// </remarks>
	GLfloat view[16]{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	/// <summary>
	/// The offset into the uniform buffer where the view matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr viewOffset{ 0 };
	GLsizeiptr const viewSize{ sizeof(view) };
	static_assert(viewSize == sizeof(GLfloat) * 4 * 4, "Check whether the uniform buffer allocation is correct.");
	/// <summary>
	/// The projection matrix to be passed to the vertex shaders.
	/// </summary>
	/// <remarks>
	/// <para>
	/// Remember that this is used in column-major order,
	/// but in the source code it looks like it is in row-major order.
	/// Therefore you need to transpose it in your mind.
	/// Fortunately all the hard-coded source code appearances
	/// are purely the identity matrix, where transposition does nothing.
	/// </para>
	/// </remarks>
	GLfloat projection[16]{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	/// <summary>
	/// The offset into the uniform buffer where the projection matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr projectionOffset{ 0 };
	GLsizeiptr const projectionSize{ sizeof(projection) };
	static_assert(projectionSize == sizeof(GLfloat) * 4 * 4, "Check whether the uniform buffer allocation is correct.");

	void InitializeOpenGLState(void)
	{
		GLubyte const* const glVendor{ glGetString(GL_VENDOR) };
		GLubyte const* const glRenderer{ glGetString(GL_RENDERER) };
		glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

		assert(glVendor != NULL);
		assert(glRenderer != NULL);

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(0.5, 2);
		glEnable(GL_POLYGON_OFFSET_FILL);

		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_POLYGON_SMOOTH);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glm::mat4 view2{ glm::lookAt(
			glm::vec3{ 0, 50, 50 },
			glm::vec3{ 0, 0, 13 },
			glm::vec3{ 0, 0, 1 }) };
		/*
		 * I deliberately assign individual elements and avoid the array
		 * block initialization syntax so that the column-major order
		 * behavior is not confused with the row-major syntax of C++.
		 */
		view[0] = view2[0][0];
		view[1] = view2[0][1];
		view[2] = view2[0][2];
		view[3] = view2[0][3];

		view[4] = view2[1][0];
		view[5] = view2[1][1];
		view[6] = view2[1][2];
		view[7] = view2[1][3];

		view[8] = view2[2][0];
		view[9] = view2[2][1];
		view[10] = view2[2][2];
		view[11] = view2[2][3];

		view[12] = view2[3][0];
		view[13] = view2[3][1];
		view[14] = view2[3][2];
		view[15] = view2[3][3];

		{
			GLfloat const quadVertices[2 * 4]
			{
				1, 1,
				-1, 1,
				-1, -1,
				1, -1,
			};
			GLsizeiptr const quadVerticesSize{ sizeof(quadVertices) };
			static_assert(quadVerticesSize == sizeof(GLfloat) * 2 * 4, "I do not know how sizeof works.");

			glGenBuffers(1, &originalVertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{
			GLuint const quadIndices[4]
			{
				0, 1, 2, 3,
			};
			GLsizeiptr const quadIndicesSize{ sizeof(quadIndices) };
			static_assert(quadIndicesSize == sizeof(GLuint) * 4, "I do not know how sizeof works.");

			glGenBuffers(1, &wingIndexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesSize, quadIndices, GL_STATIC_DRAW);
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

		wingTransformProgram = new Program{
			VertexShader{
				R"shaderText(#version 150

uniform vec2 radiusAngle;
uniform vec3 rollPitchYaw;

in vec4 vertex;

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
			{"gl_Position"}
		};
		glGenVertexArrays(1, &wingTransformVertexArray);
		glBindVertexArray(wingTransformVertexArray);
		glEnableVertexAttribArray(wingTransformProgram->getAttributeLocation("vertex"));
		glBindVertexArray(0);

		renderProgram = new Program{
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
		};
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
				GLuint uniformIndices[3]{ 0, 0, 0, };
				GLchar const* const names[3]{ "model", "view", "projection" };
				glGetUniformIndices(programId, 3, names, uniformIndices);

				GLint uniformOffsets[3]{ 0, 0, 0, };
				glGetActiveUniformsiv(programId, 3, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
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

		glBindBuffer(GL_UNIFORM_BUFFER, modelViewProjectionUniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, modelOffset, modelSize, model);
		glBufferSubData(GL_UNIFORM_BUFFER, viewOffset, viewSize, view);
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, projectionSize, projection);
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

		delete wingTransformProgram;
		wingTransformProgram = nullptr;
		delete renderProgram;
		renderProgram = nullptr;
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
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, nullptr, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &wingColorBuffer);
			glGenBuffers(1, &wingEdgeColorBuffer);
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

		wing_list::const_reference wing{ wings.emplace_front(wingVertexBuffer,
			wingColorBuffer,
			wingEdgeColorBuffer,
			deltaAngle, deltaZ) };

		{
			GLfloat const colorData[3 * 4]{
				red, green, blue,
				red, green, blue,
				red, green, blue,
				red, green, blue,
			};
			GLsizeiptr const colorDataSize{ sizeof(colorData) };
			static_assert(colorDataSize == sizeof(GLfloat) * 3 * 4, "I do not know how sizeof works.");
			glBindBuffer(GL_ARRAY_BUFFER, wingColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, colorDataSize, colorData, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{
			Color<GLfloat> const wingEdgeColor{ Color<GLfloat>::WHITE };
			GLfloat const edgeRed{ wingEdgeColor.getRed() };
			GLfloat const edgeGreen{ wingEdgeColor.getGreen() };
			GLfloat const edgeBlue{ wingEdgeColor.getBlue() };
			GLfloat const edgeColorData[3 * 4]{
				edgeRed, edgeGreen, edgeBlue,
				edgeRed, edgeGreen, edgeBlue,
				edgeRed, edgeGreen, edgeBlue,
				edgeRed, edgeGreen, edgeBlue,
			};
			GLsizeiptr const edgeColorDataSize{ sizeof(edgeColorData) };
			static_assert(edgeColorDataSize == sizeof(GLfloat) * 3 * 4, "I do not know how sizeof works.");
			glBindBuffer(GL_ARRAY_BUFFER, wingEdgeColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, edgeColorDataSize, edgeColorData, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		wingTransformProgram->useProgram();
		glUniform2f(wingTransformProgram->getUniformLocation("radiusAngle"), radius, angle);
		glUniform3f(wingTransformProgram->getUniformLocation("rollPitchYaw"), roll, pitch, yaw);

		glBindVertexArray(wingTransformVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
		glVertexAttribPointer(wingTransformProgram->getAttributeLocation("vertex"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, wingVertexBuffer);

		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, 4);
		glEndTransformFeedback();

		glBindVertexArray(0);
	}

	void DrawFrame(void)
	{
		renderProgram->useProgram();
		GLuint const vertexAttribLocation{ renderProgram->getAttributeLocation("vertex") };
		GLuint const colorAttribLocation{ renderProgram->getAttributeLocation("color") };

		GLuint deltaAttribBuffer{ 0 };
		glGenBuffers(1, &deltaAttribBuffer);

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

			glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
		}

		glFlush();

		glDeleteBuffers(1, &deltaAttribBuffer);

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
		
		GLfloat const left{ -20 * xmult };
		GLfloat const right{ 20 * xmult };
		GLfloat const bottom{ -20 * ymult };
		GLfloat const top{ 20 * ymult };
		GLfloat const nearZ{ 15 };
		GLfloat const farZ{ 105 };

		GLfloat const viewWidth{ right - left };
		GLfloat const viewHeight{ top - bottom };
		GLfloat const viewDepth{ farZ - nearZ };

		/*
		 * I deliberately assign individual elements and avoid the array
		 * block initialization syntax so that the column-major order
		 * behavior is not confused with the row-major syntax of C++.
		 */
		projection[0] = static_cast<GLfloat>(2) / viewWidth;
		projection[1] = 0;
		projection[2] = 0;
		projection[3] = 0;

		projection[4] = 0;
		projection[5] = static_cast<GLfloat>(2) / viewHeight;
		projection[6] = 0;
		projection[7] = 0;

		projection[8] = 0;
		projection[9] = 0;
		projection[10] = static_cast<GLfloat>(-2) / viewDepth;
		projection[11] = 0;

		projection[12] = -(right + left) / viewWidth;
		projection[13] = -(top + bottom) / viewHeight;
		projection[14] = -(farZ + nearZ) / viewDepth;
		projection[15] = static_cast<GLfloat>(1);

		glBindBuffer(GL_UNIFORM_BUFFER, modelViewProjectionUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, projectionSize, projection);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void Resize(GLsizei width, GLsizei height)
	{
		glViewport(0, 0, width, height);
		Ortho(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	}

}
