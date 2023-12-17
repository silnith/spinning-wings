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

	typedef std::deque<Wing> wing_list;

	size_t const numWings{ 40 };

	GLint glMajorVersion{ 1 };
	GLint glMinorVersion{ 0 };

	wing_list wings{};

	CurveGenerator radiusCurve{ 10.0f, -15.0f, 15.0f, false, 0.1f, 0.01f, 150 };
	CurveGenerator angleCurve{ CurveGenerator::createGeneratorForAngles(0.0f, 2.0f, 0.05f, 120) };
	CurveGenerator deltaAngleCurve{ CurveGenerator::createGeneratorForAngles(15.0f, 0.2f, 0.02f, 80) };
	CurveGenerator deltaZCurve{ 0.5f, 0.4f, 0.7f, false, 0.01f, 0.001f, 200 };
	CurveGenerator rollCurve{ CurveGenerator::createGeneratorForAngles(0.0f, 1.0f, 0.25f, 80) };
	CurveGenerator pitchCurve{ CurveGenerator::createGeneratorForAngles(0.0f, 2.0f, 0.25f, 40) };
	CurveGenerator yawCurve{ CurveGenerator::createGeneratorForAngles(0.0f, 1.5f, 0.25f, 50) };
	CurveGenerator redCurve{ CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 95) };
	CurveGenerator greenCurve{ CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 40) };
	CurveGenerator blueCurve{ CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 70) };

	Program* wingTransformProgram{ nullptr };
	Program* renderingProgram{ nullptr };

	/// <summary>
	/// The initial untransformed vertices for a single quad.
	/// After binding, enable using <c>glVertexAttribPointer(..., 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0)</c>.
	/// </summary>
	GLuint originalVertexBuffer{ 0 };
	/// <summary>
	/// The indices into <c>originalVertexBuffer</c>.
	/// </summary>
	GLuint wingIndexBuffer{ 0 };

	GLfloat model[16]{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	GLfloat view[16]{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	GLfloat projection[16]{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

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

		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_POLYGON_SMOOTH);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glm::mat4 view2{ glm::lookAt(
			glm::vec3{ 0, 50, 50 },
			glm::vec3{ 0, 0, 13 },
			glm::vec3{ 0, 0, 1 }) };
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
			GLfloat const quadVertices[8]
			{
				1, 1,
				-1, 1,
				-1, -1,
				1, -1,
			};
			GLsizeiptr const quadVerticesSize{ sizeof(quadVertices) };
			static_assert(quadVerticesSize == sizeof(GLfloat) * 2 * 4, "Size of quad vertices array is not as expected.");

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

		// TODO: #version 150
		wingTransformProgram = new Program{
			VertexShader{
				R"shaderText(#version 130

in vec2 vertex;
in vec2 radiusAngle;
in vec3 rollPitchYaw;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    mat4 wingTransformation = rotate(angle, vec3(0, 0, 1))
                              * translate(vec3(radius, 0, 0))
                              * rotate(-yaw, vec3(0, 0, 1))
                              * rotate(-pitch, vec3(0, 1, 0))
                              * rotate(roll, vec3(1, 0, 0));
    gl_Position = wingTransformation * vec4(vertex, 0, 1);
}
)shaderText",
				rotateMatrixFunctionDeclaration,
				translateMatrixFunctionDeclaration,
			},
			std::vector<std::string>{"gl_Position"}
		};

		renderingProgram = new Program{
			VertexShader{
				R"shaderText(#version 130

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 vertex;
in vec3 color;
in vec2 deltaZ;

smooth out vec4 varyingColor;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

void main() {
    float deltaAngle = deltaZ[0];
    float deltaZ = deltaZ[1];

    mat4 modelViewProjection = projection * view * model;

    varyingColor = vec4(color, 1);
    gl_Position = modelViewProjection
                  * translate(vec3(0, 0, deltaZ))
                  * rotate(deltaAngle, vec3(0, 0, 1))
                  * vertex;
}
)shaderText",
				rotateMatrixFunctionDeclaration,
				translateMatrixFunctionDeclaration,
			},
			FragmentShader{
				R"shaderText(#version 130

in vec4 varyingColor;

out vec4 fragmentColor;

void main() {
    fragmentColor = varyingColor;
}
)shaderText",
			},
			"fragmentColor"
		};
	}

	void CleanupOpenGLState(void)
	{
		for (Wing const& wing : wings)
		{
			GLuint const vertexBuffer{ wing.getVertexBuffer() };
			GLuint const colorBuffer{ wing.getColorBuffer() };
			GLuint const edgeColorBuffer{ wing.getEdgeColorBuffer() };
			glDeleteBuffers(1, &vertexBuffer);
			glDeleteBuffers(1, &colorBuffer);
			glDeleteBuffers(1, &edgeColorBuffer);
		}

		glDeleteBuffers(1, &wingIndexBuffer);
		glDeleteBuffers(1, &originalVertexBuffer);

		delete wingTransformProgram;
		wingTransformProgram = nullptr;
		delete renderingProgram;
		renderingProgram = nullptr;
	}

	void AdvanceAnimation(void)
	{
		GLuint wingVertexBuffer{ 0 };
		GLuint wingColorBuffer{ 0 };
		GLuint wingEdgeColorBuffer{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			glGenBuffers(1, &wingVertexBuffer);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, wingVertexBuffer);
			glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(GLfloat) * 4 * 4, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

			glGenBuffers(1, &wingColorBuffer);
			glGenBuffers(1, &wingEdgeColorBuffer);
		}
		else
		{
			{
				Wing const& lastWing{ wings.back() };
				wingVertexBuffer = lastWing.getVertexBuffer();
				wingColorBuffer = lastWing.getColorBuffer();
				wingEdgeColorBuffer = lastWing.getEdgeColorBuffer();
			}
			wings.pop_back();
		}

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

		Wing const& wing{ wings.emplace_front(wingVertexBuffer,
			wingColorBuffer,
			wingEdgeColorBuffer,
			deltaAngle, deltaZ) };

		{
			GLfloat const colorData[12]{
				red, green, blue,
				red, green, blue,
				red, green, blue,
				red, green, blue,
			};
			GLsizeiptr const colorDataSize{ sizeof(colorData) };
			static_assert(colorDataSize == sizeof(GLfloat) * 3 * 4, "");
			glBindBuffer(GL_ARRAY_BUFFER, wingColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, colorDataSize, colorData, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{
			Color const wingEdgeColor{ Color::WHITE };
			GLfloat const red{ wingEdgeColor.getRed() };
			GLfloat const green{ wingEdgeColor.getGreen() };
			GLfloat const blue{ wingEdgeColor.getBlue() };
			GLfloat const edgeColorData[12]{
				red, green, blue,
				red, green, blue,
				red, green, blue,
				red, green, blue,
			};
			GLsizeiptr const edgeColorDataSize{ sizeof(edgeColorData) };
			static_assert(edgeColorDataSize == sizeof(GLfloat) * 3 * 4, "");
			glBindBuffer(GL_ARRAY_BUFFER, wingEdgeColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, edgeColorDataSize, edgeColorData, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		wingTransformProgram->useProgram();
		GLuint const vertexAttribLocation{ wingTransformProgram->getAttributeLocation("vertex") };
		GLuint const radiusAngleAttribLocation{ wingTransformProgram->getAttributeLocation("radiusAngle") };
		GLuint const rollPitchYawAttribLocation{ wingTransformProgram->getAttributeLocation("rollPitchYaw") };

		GLuint angleRadiusBuffer{ 0 };
		GLuint rollPitchYawBuffer{ 0 };

		{
			GLfloat const angleRadiusData[8]{
				radius, angle,
				radius, angle,
				radius, angle,
				radius, angle,
			};
			GLsizeiptr const angleRadiusDataSize{ sizeof(angleRadiusData) };
			static_assert(angleRadiusDataSize == sizeof(GLfloat) * 2 * 4, "");
			glGenBuffers(1, &angleRadiusBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, angleRadiusBuffer);
			glBufferData(GL_ARRAY_BUFFER, angleRadiusDataSize, angleRadiusData, GL_STATIC_DRAW);
			glVertexAttribPointer(radiusAngleAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{
			GLfloat const rollPitchYawData[12]{
				roll, pitch, yaw,
				roll, pitch, yaw,
				roll, pitch, yaw,
				roll, pitch, yaw,
			};
			GLsizeiptr const rollPitchYawDataSize{ sizeof(rollPitchYawData) };
			static_assert(rollPitchYawDataSize == sizeof(GLfloat) * 3 * 4, "");
			glGenBuffers(1, &rollPitchYawBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, rollPitchYawBuffer);
			glBufferData(GL_ARRAY_BUFFER, rollPitchYawDataSize, rollPitchYawData, GL_STATIC_DRAW);
			glVertexAttribPointer(rollPitchYawAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		//glEnable(GL_RASTERIZER_DISCARD);

		glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
		glVertexAttribPointer(vertexAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, wingVertexBuffer);

		glEnableVertexAttribArray(vertexAttribLocation);
		glEnableVertexAttribArray(radiusAngleAttribLocation);
		glEnableVertexAttribArray(rollPitchYawAttribLocation);
		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, 4);
		glEndTransformFeedback();
		glDisableVertexAttribArray(rollPitchYawAttribLocation);
		glDisableVertexAttribArray(radiusAngleAttribLocation);
		glDisableVertexAttribArray(vertexAttribLocation);

		//glDisable(GL_RASTERIZER_DISCARD);

		glDeleteBuffers(1, &rollPitchYawBuffer);
		glDeleteBuffers(1, &angleRadiusBuffer);
	}

	void DrawFrame(void)
	{
		renderingProgram->useProgram();
		GLuint const vertexAttribLocation{ renderingProgram->getAttributeLocation("vertex") };
		GLuint const colorAttribLocation{ renderingProgram->getAttributeLocation("color") };
		GLuint const deltaZAttribLocation{ renderingProgram->getAttributeLocation("deltaZ") };

		GLint const modelUniformLocation{ renderingProgram->getUniformLocation("model") };
		GLint const viewUniformLocation{ renderingProgram->getUniformLocation("view") };
		GLint const projectionUniformLocation{ renderingProgram->getUniformLocation("projection") };

		glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, model);
		glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, view);
		glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, projection);

		GLuint deltaAttribBuffer{ 0 };

		glGenBuffers(1, &deltaAttribBuffer);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (Wing const& wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			{
				GLfloat const deltaData[8]{
					deltaAngle, deltaZ,
					deltaAngle, deltaZ,
					deltaAngle, deltaZ,
					deltaAngle, deltaZ,
				};
				GLsizeiptr const deltaDataSize{ sizeof(deltaData) };
				static_assert(deltaDataSize == sizeof(GLfloat) * 2 * 4, "");
				glBindBuffer(GL_ARRAY_BUFFER, deltaAttribBuffer);
				glBufferData(GL_ARRAY_BUFFER, deltaDataSize, deltaData, GL_STATIC_DRAW);
				glVertexAttribPointer(deltaZAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, wing.getVertexBuffer());
			glVertexAttribPointer(vertexAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getEdgeColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(vertexAttribLocation);
			glEnableVertexAttribArray(colorAttribLocation);
			glEnableVertexAttribArray(deltaZAttribLocation);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);
			glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(deltaZAttribLocation);
			glDisableVertexAttribArray(colorAttribLocation);
			glDisableVertexAttribArray(vertexAttribLocation);
		}
		glDisable(GL_POLYGON_OFFSET_FILL);

		glFlush();

		glDeleteBuffers(1, &deltaAttribBuffer);
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
	}

	void Resize(GLsizei width, GLsizei height)
	{
		glViewport(0, 0, width, height);
		Ortho(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	}

}
