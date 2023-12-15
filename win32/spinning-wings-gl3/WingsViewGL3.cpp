#include "WingsViewGL3.h"

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

	GLuint glMajorVersion{ 1 };
	GLuint glMinorVersion{ 0 };

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
	/// After binding, enable using <c>glVertexPointer(2, GL_FLOAT, 0, 0)</c>.
	/// </summary>
	GLuint originalVertexBuffer{ 0 };
	/// <summary>
	/// The indices into <c>originalVertexBuffer</c>.
	/// </summary>
	GLuint wingIndexBuffer{ 0 };

	void InitializeOpenGLState(void)
	{
		GLubyte const* const glVendor{ glGetString(GL_VENDOR) };
		GLubyte const* const glRenderer{ glGetString(GL_RENDERER) };
		GLint major{ 0 };
		GLint minor{ 0 };
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		assert(glVendor != NULL);
		assert(glRenderer != NULL);

		glMajorVersion = static_cast<GLuint>(major);
		glMinorVersion = static_cast<GLuint>(minor);

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(0.5, 2);

		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_POLYGON_SMOOTH);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glLoadIdentity();
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);

		{
			GLfloat const quadVertices[8]
			{
				1, 1,
				-1, 1,
				-1, -1,
				1, -1,
			};
			GLsizeiptr const quadVerticesSize{ sizeof(quadVertices) };
			static_assert(quadVerticesSize == sizeof(GLfloat) * 8, "Size of quad vertices array is not as expected.");

			glGenBuffers(1, &originalVertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
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

in vec4 vertex;
in vec2 deltaZ;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

void main() {
    float deltaAngle = deltaZ[0];
    float deltaZ = deltaZ[1];

    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix
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

void main() {
    gl_FragColor = gl_Color;
    gl_FragDepth = gl_FragCoord.z;
}
)shaderText",
			}
		};
	}

	void CleanupOpenGLState(void)
	{
		for (Wing const& wing : wings)
		{
			GLuint const vertexBuffer{ wing.getVertexBuffer() };
			glDeleteBuffers(1, &vertexBuffer);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

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
		if (wings.empty() || wings.size() < numWings)
		{
			glGenBuffers(1, &wingVertexBuffer);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, wingVertexBuffer);
			glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(GLfloat) * 4 * 4, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
		}
		else
		{
			Wing const& lastWing{ wings.back() };
			wingVertexBuffer = lastWing.getVertexBuffer();
			wings.pop_back();
		}
		Wing const& wing{ wings.emplace_front(wingVertexBuffer,
			radiusCurve.getNextValue(), angleCurve.getNextValue(),
			deltaAngleCurve.getNextValue(), deltaZCurve.getNextValue(),
			rollCurve.getNextValue(), pitchCurve.getNextValue(), yawCurve.getNextValue(),
			Color{ redCurve.getNextValue(), greenCurve.getNextValue(), blueCurve.getNextValue() },
			Color::WHITE) };

		wingTransformProgram->useProgram();
		GLuint const vertexAttribLocation{ wingTransformProgram->getAttributeLocation("vertex") };
		GLuint const radiusAngleAttribLocation{ wingTransformProgram->getAttributeLocation("radiusAngle") };
		GLuint const rollPitchYawAttribLocation{ wingTransformProgram->getAttributeLocation("rollPitchYaw") };

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, wingVertexBuffer);

		//glEnable(GL_RASTERIZER_DISCARD);

		glBeginTransformFeedback(GL_POINTS);
		glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
		glVertexAttribPointer(vertexAttribLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glVertexAttrib2f(radiusAngleAttribLocation, wing.getRadius(), wing.getAngle());
		glVertexAttrib3f(rollPitchYawAttribLocation, wing.getRoll(), wing.getPitch(), wing.getYaw());
		glEnableVertexAttribArray(vertexAttribLocation);
		glDrawArrays(GL_POINTS, 0, 4);
		glDisableVertexAttribArray(vertexAttribLocation);
		glEndTransformFeedback();

		//glDisable(GL_RASTERIZER_DISCARD);
	}

	void DrawFrame(void)
	{
		renderingProgram->useProgram();
		GLuint const vertexAttribLocation{ renderingProgram->getAttributeLocation("vertex") };
		GLuint const deltaZAttribLocation{ renderingProgram->getAttributeLocation("deltaZ") };

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			GLfloat deltaZ{ 0 };
			GLfloat deltaAngle{ 0 };
			for (Wing const& wing : wings) {
				deltaZ += wing.getDeltaZ();
				deltaAngle += wing.getDeltaAngle();

				GLuint const wingVertexBuffer{ wing.getVertexBuffer() };

				glBindBuffer(GL_ARRAY_BUFFER, wingVertexBuffer);
				glVertexAttribPointer(vertexAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);

				Color const& edgeColor{ wing.getEdgeColor() };
				// TODO: deprecated
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
				glEnableVertexAttribArray(vertexAttribLocation);
				glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
				glDisableVertexAttribArray(vertexAttribLocation);
			}
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (Wing const& wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			GLuint const wingVertexBuffer{ wing.getVertexBuffer() };

			glBindBuffer(GL_ARRAY_BUFFER, wingVertexBuffer);
			glVertexAttribPointer(vertexAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);

			Color const& color{ wing.getColor() };
			// TODO: deprecated
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
			glEnableVertexAttribArray(vertexAttribLocation);
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
			glDisableVertexAttribArray(vertexAttribLocation);
		}
		glDisable(GL_POLYGON_OFFSET_FILL);

		glFlush();
	}

	void Resize(GLsizei width, GLsizei height)
	{
		GLdouble xmult{ 1.0 };
		GLdouble ymult{ 1.0 };
		if (width > height)
		{
			xmult = static_cast<GLdouble>(width) / static_cast<GLdouble>(height);
		}
		else
		{
			ymult = static_cast<GLdouble>(height) / static_cast<GLdouble>(width);
		}

		glViewport(0, 0, width, height);
		// TODO: deprecated
		glMatrixMode(GL_PROJECTION);
		// TODO: deprecated
		glLoadIdentity();
		// TODO: deprecated
		glOrtho(static_cast<GLdouble>(-20) * xmult, static_cast<GLdouble>(20) * xmult,
			static_cast<GLdouble>(-20) * ymult, static_cast<GLdouble>(20) * ymult,
			static_cast<GLdouble>(35), static_cast<GLdouble>(105));
		// TODO: deprecated
		glMatrixMode(GL_MODELVIEW);
		// check GL errors
	}

}
