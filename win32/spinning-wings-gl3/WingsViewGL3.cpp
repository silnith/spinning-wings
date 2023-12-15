#include "WingsViewGL3.h"

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

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

	GLfloat const quadVertices[12]
	{
		1, 1, 0,
		-1, 1, 0,
		-1, -1, 0,
		1, -1, 0,
	};
	GLsizeiptr const quadVerticesSize{ sizeof(GLfloat) * 12 };
	GLuint const quadIndices[4]
	{
		0, 1, 2, 3,
	};
	GLsizeiptr const quadIndicesSize{ sizeof(GLuint) * 4 };

	static_assert(quadVerticesSize == sizeof(quadVertices), "Size of quad vertices array is not as expected.");
	static_assert(quadIndicesSize == sizeof(quadIndices), "I do not know how sizeof works.");

	GLuint wingVerticesBufferObject{ 0 };
	GLuint wingIndicesBufferObject{ 0 };

	GLuint deltaZAttribLocation{ 0 };
	GLuint radiusAngleAttribLocation{ 0 };
	GLuint rollPitchYawAttribLocation{ 0 };

	void InitializeOpenGLState(void)
	{
		GLubyte const* const glVendor{ glGetString(GL_VENDOR) };
		GLubyte const* const glRenderer{ glGetString(GL_RENDERER) };
		GLubyte const* const glExtensions{ glGetString(GL_EXTENSIONS) };
		GLint major{ 0 };
		GLint minor{ 0 };
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		assert(glVendor != NULL);
		assert(glRenderer != NULL);
		assert(glExtensions != NULL);

		glMajorVersion = static_cast<GLuint>(major);
		glMinorVersion = static_cast<GLuint>(minor);

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(0.5, 2);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth(1.0);

		glEnable(GL_POLYGON_SMOOTH);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glLoadIdentity();
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);

		glGenBuffers(1, &wingVerticesBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, wingVerticesBufferObject);
		glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		// glColorPointer();
		// glEdgeFlagPointer();
		// glVertexAttribPointer();

		glGenBuffers(1, &wingIndicesBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesSize, quadIndices, GL_STATIC_DRAW);

		wingTransformProgram = new Program{
			VertexShader{
				"#version 130",
				"",
				"in vec2 radiusAngle;",
				"in vec3 rollPitchYaw;",
				"",
				"mat4 rotate(in float angle, in vec3 axis) {",
				"    float c = cos(radians(angle));",
				"    float s = sin(radians(angle));",
				"",
				"    mat3 initial = outerProduct(axis, axis) * (1 - c);",
				"",
				"    mat3 c_part = mat3(c);",
				"",
				"    mat3 s_part = mat3(0, axis.z, -axis.y, -axis.z, 0, axis.x, axis.y, -axis.x, 0) * s;",
				"",
				"    mat3 temp = initial + c_part + s_part;",
				"",
				"    mat4 rotation = mat4(1.0);",
				"    rotation[0].xyz = temp[0];",
				"    rotation[1].xyz = temp[1];",
				"    rotation[2].xyz = temp[2];",
				"",
				"    return rotation;",
				"}",
				"",
				"mat4 translate(in vec3 move) {",
				"    mat4 trans = mat4(1.0);",
				"    trans[3].xyz = move;",
				"    return trans;",
				"}",
				"",
				"void main() {",
				"    float radius = radiusAngle[0];",
				"    float angle = radiusAngle[1];",
				"    float roll = rollPitchYaw[0];",
				"    float pitch = rollPitchYaw[1];",
				"    float yaw = rollPitchYaw[2];",
				"    ",
				"    mat4 angle_rot = rotate(angle, vec3(0, 0, 1));",
				"    mat4 radius_trans = translate(vec3(radius, 0, 0));",
				"    ",
				"    mat4 yaw_rot = rotate(-yaw, vec3(0, 0, 1));",
				"    mat4 pitch_rot = rotate(-pitch, vec3(0, 1, 0));",
				"    mat4 roll_rot = rotate(roll, vec3(1, 0, 0));",
				"    ",
				"    gl_Position = angle_rot * radius_trans * yaw_rot * pitch_rot * roll_rot * gl_Vertex;",
				"}",
			},
			std::vector<std::string>{"gl_Position"}
		};
		radiusAngleAttribLocation = wingTransformProgram->getAttributeLocation("radiusAngle");
		rollPitchYawAttribLocation = wingTransformProgram->getAttributeLocation("rollPitchYaw");

		renderingProgram = new Program{
			VertexShader{
				"#version 130",
				"",
				"attribute vec2 deltaZ;",
				"",
				"mat4 rotate(in float angle, in vec3 axis) {",
				"    float c = cos(radians(angle));",
				"    float s = sin(radians(angle));",
				"",
				"    mat3 initial = outerProduct(axis, axis) * (1 - c);",
				"",
				"    mat3 c_part = mat3(c);",
				"",
				"    mat3 s_part = mat3(0, axis.z, -axis.y, -axis.z, 0, axis.x, axis.y, -axis.x, 0) * s;",
				"",
				"    mat3 temp = initial + c_part + s_part;",
				"",
				"    mat4 rotation = mat4(1.0);",
				"    rotation[0].xyz = temp[0];",
				"    rotation[1].xyz = temp[1];",
				"    rotation[2].xyz = temp[2];",
				"",
				"    return rotation;",
				"}",
				"",
				"mat4 translate(in vec3 move) {",
				"    mat4 trans = mat4(1.0);",
				"    trans[3].xyz = move;",
				"    return trans;",
				"}",
				"",
				"void main() {",
				"    float deltaAngle = deltaZ[0];",
				"    float deltaZ = deltaZ[1];",
				"    ",
				"    mat4 deltaZ_trans = translate(vec3(0, 0, deltaZ));",
				"    mat4 deltaAngle_rot = rotate(deltaAngle, vec3(0, 0, 1));",
				"    ",
				"    gl_FrontColor = gl_Color;",
				"    gl_BackColor = gl_Color;",
				"    gl_Position = gl_ModelViewProjectionMatrix * deltaZ_trans * deltaAngle_rot * gl_Vertex;",
				"}",
			},
			FragmentShader{
				"#version 130",
				"",
				"void main() {",
				"    gl_FragColor = gl_Color;",
				"    gl_FragDepth = gl_FragCoord.z;",
				"}",
			}
		};
		deltaZAttribLocation = renderingProgram->getAttributeLocation("deltaZ");
	}

	void CleanupOpenGLState(void)
	{
		for (Wing const& wing : wings)
		{
			GLuint const buffer{ wing.getGLDisplayList() };
			glDeleteBuffers(1, &buffer);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &wingIndicesBufferObject);
		glDeleteBuffers(1, &wingVerticesBufferObject);

		delete wingTransformProgram;
		wingTransformProgram = nullptr;
		delete renderingProgram;
		renderingProgram = nullptr;
	}

	void AdvanceAnimation(void)
	{
		GLuint transformFeedbackBuffer{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			glGenBuffers(1, &transformFeedbackBuffer);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, transformFeedbackBuffer);
			glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(GLfloat) * 4 * 4, nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
		}
		else
		{
			Wing const& lastWing{ wings.back() };
			transformFeedbackBuffer = lastWing.getGLDisplayList();
			wings.pop_back();
		}
		Wing const& wing{ wings.emplace_front(transformFeedbackBuffer,
			radiusCurve.getNextValue(), angleCurve.getNextValue(),
			deltaAngleCurve.getNextValue(), deltaZCurve.getNextValue(),
			rollCurve.getNextValue(), pitchCurve.getNextValue(), yawCurve.getNextValue(),
			Color{ redCurve.getNextValue(), greenCurve.getNextValue(), blueCurve.getNextValue() },
			Color::WHITE) };

		wingTransformProgram->useProgram();

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, transformFeedbackBuffer);

		//glEnable(GL_RASTERIZER_DISCARD);

		glBeginTransformFeedback(GL_POINTS);
		glBindBuffer(GL_ARRAY_BUFFER, wingVerticesBufferObject);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glVertexAttrib2f(radiusAngleAttribLocation, wing.getRadius(), wing.getAngle());
		glVertexAttrib3f(rollPitchYawAttribLocation, wing.getRoll(), wing.getPitch(), wing.getYaw());
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_POINTS, 0, 4);
		//glDrawElements(GL_POINTS, 4, GL_UNSIGNED_INT, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glEndTransformFeedback();

		//glDisable(GL_RASTERIZER_DISCARD);
	}

	void DrawFrame(void)
	{
		renderingProgram->useProgram();
		deltaZAttribLocation = renderingProgram->getAttributeLocation("deltaZ");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			GLfloat deltaZ{ 0 };
			GLfloat deltaAngle{ 0 };
			for (Wing const& wing : wings) {
				deltaZ += wing.getDeltaZ();
				deltaAngle += wing.getDeltaAngle();

				GLuint const wingVertexBufferObject{ wing.getGLDisplayList() };

				glBindBuffer(GL_ARRAY_BUFFER, wingVertexBufferObject);
				glVertexPointer(4, GL_FLOAT, 0, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);

				Color const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
				glEnableClientState(GL_VERTEX_ARRAY);
				glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (Wing const& wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			GLuint const wingVertexBufferObject{ wing.getGLDisplayList() };

			glBindBuffer(GL_ARRAY_BUFFER, wingVertexBufferObject);
			glVertexPointer(4, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);

			Color const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
			glEnableClientState(GL_VERTEX_ARRAY);
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
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
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(static_cast<GLdouble>(-20) * xmult, static_cast<GLdouble>(20) * xmult,
			static_cast<GLdouble>(-20) * ymult, static_cast<GLdouble>(20) * ymult,
			static_cast<GLdouble>(35), static_cast<GLdouble>(105));
		glMatrixMode(GL_MODELVIEW);
		// check GL errors
	}

}