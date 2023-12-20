#include "WingsViewGL2.h"

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

namespace silnith::wings::gl2
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

	Program* glslProgram{ nullptr };

	bool hasOpenGL(GLuint major, GLuint minor)
	{
		return (glMajorVersion > major)
			|| (glMajorVersion == major && glMinorVersion >= minor);
	}

	void ParseOpenGLVersion(GLubyte const* glVersion)
	{
		std::istringstream versionStringInput{ std::string{ reinterpret_cast<char const*>(glVersion) } };
		//std::basic_istringstream<GLubyte> versionStringInput{ std::basic_string<GLubyte>{glVersion} };

		versionStringInput >> glMajorVersion;
		GLubyte period;
		versionStringInput >> period;
		assert(period == '.');
		versionStringInput >> glMinorVersion;
	}

	// The GL display list for rendering a single quad.
	// This is used for the GL 1.0 rendering path.
	GLuint quadDisplayList{ 0 };

	void DrawQuadGL1_0(void)
	{
		glCallList(quadDisplayList);
	}

	// The rendering path for rendering a single quad.
	// This points to the appropriate rendering path for the
	// active version of the GL.
	void (*drawQuad)(void) { DrawQuadGL1_0 };

	void CleanupDrawQuadGL1_0(void)
	{
		glDeleteLists(quadDisplayList, 1);
	}

	void (*cleanupDrawQuad)(void) { CleanupDrawQuadGL1_0 };

	void InitializeDrawQuadGL1_0(void)
	{
		quadDisplayList = glGenLists(1);
		glNewList(quadDisplayList, GL_COMPILE);
		glBegin(GL_QUADS);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glEnd();
		glEndList();

		drawQuad = DrawQuadGL1_0;
		cleanupDrawQuad = CleanupDrawQuadGL1_0;
	}

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

	void DrawQuadGL1_1(void)
	{
		/*
		 * EnableClientState is executed immediately, it is not compiled into
		 * a display list.  When creating a display list,
		 * DrawElements dereferences vertex pointers according to the client
		 * state, and the dereferenced vertices are compiled into the
		 * display list.
		 */
		glEnableClientState(GL_VERTEX_ARRAY);

		/*
		 * Elements are interpreted according to the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, quadIndices);

		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void CleanupDrawQuadGL1_1(void)
	{
	}

	void InitializeDrawQuadGL1_1(void)
	{
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 */
		glVertexPointer(3, GL_FLOAT, 0, quadVertices);

		drawQuad = DrawQuadGL1_1;
		cleanupDrawQuad = CleanupDrawQuadGL1_1;
	}

	void DrawQuadGL1_5(void)
	{
		/*
		 * EnableClientState is executed immediately, it is not compiled into
		 * a display list.  When compiled into a display list,
		 * DrawElements dereferences vertex pointers according to the client
		 * state, and the dereferenced vertices are compiled into the
		 * display list.
		 */
		glEnableClientState(GL_VERTEX_ARRAY);

		/*
		 * If there is an ELEMENT_ARRAY_BUFFER bound, then the last parameter
		 * to DrawElements is interpreted as an index/offset into the
		 * ELEMENT_ARRAY_BUFFER.
		 * 
		 * Elements of the ELEMENT_ARRAY_BUFFER are interpreted according to
		 * the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
	}

	GLuint wingBufferObject{ 0 };
	GLuint wingIndicesBufferObject{ 0 };

	void CleanupDrawQuadGL1_5(void)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &wingIndicesBufferObject);
		glDeleteBuffers(1, &wingBufferObject);
	}

	void InitializeDrawQuadGL1_5(void)
	{
		glGenBuffers(1, &wingBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, wingBufferObject);
		glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 * 
		 * If there is an ARRAY_BUFFER bound, then the last parameter is interpreted
		 * as an index/offset into the ARRAY_BUFFER.
		 */
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glGenBuffers(1, &wingIndicesBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesSize, quadIndices, GL_STATIC_DRAW);

		drawQuad = DrawQuadGL1_5;
		cleanupDrawQuad = CleanupDrawQuadGL1_5;
	}

	GLuint deltaZAttribLocation{ 0 };
	GLuint radiusAngleAttribLocation{ 0 };
	GLuint rollPitchYawAttribLocation{ 0 };

	void InitializeOpenGLState(void)
	{
		GLubyte const* const glVendor{ glGetString(GL_VENDOR) };
		GLubyte const* const glRenderer{ glGetString(GL_RENDERER) };
		GLubyte const* const glVersion{ glGetString(GL_VERSION) };
		GLubyte const* const glExtensions{ glGetString(GL_EXTENSIONS) };

		assert(glVendor != NULL);
		assert(glRenderer != NULL);
		assert(glVersion != NULL);
		assert(glExtensions != NULL);

		ParseOpenGLVersion(glVersion);

		glEnable(GL_DEPTH_TEST);
		if (hasOpenGL(1, 1))
		{
			glPolygonOffset(-0.5, -2);
		}

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

		if (hasOpenGL(1, 5))
		{
			InitializeDrawQuadGL1_5();
		}
		else if (hasOpenGL(1, 1))
		{
			InitializeDrawQuadGL1_1();
		}
		else
		{
			InitializeDrawQuadGL1_0();
		}

		if (hasOpenGL(2, 1))
		{
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

			std::vector<std::string> vertexSources{
				R"shaderText(#version 120

attribute vec2 deltaZ;
attribute vec2 radiusAngle;
attribute vec3 rollPitchYaw;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float deltaAngle = deltaZ[0];
    float deltaZ = deltaZ[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    mat4 deltaZ_trans = translate(vec3(0, 0, deltaZ));
    mat4 deltaAngle_rot = rotate(deltaAngle, vec3(0, 0, 1));

    mat4 angle_rot = rotate(angle, vec3(0, 0, 1));
    mat4 radius_trans = translate(vec3(radius, 0, 0));

    mat4 yaw_rot = rotate(-yaw, vec3(0, 0, 1));
    mat4 pitch_rot = rotate(-pitch, vec3(0, 1, 0));
    mat4 roll_rot = rotate(roll, vec3(1, 0, 0));

    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * deltaZ_trans * deltaAngle_rot * angle_rot * radius_trans * yaw_rot * pitch_rot * roll_rot * gl_Vertex;
}
)shaderText",
				rotateMatrixFunctionDeclaration,
				translateMatrixFunctionDeclaration,
			};

			std::vector<std::string> fragmentSources{
				R"shaderText(#version 120

void main() {
    gl_FragColor = gl_Color;
    gl_FragDepth = gl_FragCoord.z;
}
)shaderText",
			};

			glslProgram = new Program{ VertexShader{ vertexSources }, FragmentShader{ fragmentSources } };

			deltaZAttribLocation = glslProgram->getAttributeLocation("deltaZ");
			radiusAngleAttribLocation = glslProgram->getAttributeLocation("radiusAngle");
			rollPitchYawAttribLocation = glslProgram->getAttributeLocation("rollPitchYaw");

			glslProgram->useProgram();
		}
	}

	void CleanupOpenGLState(void)
	{
		for (Wing const& wing : wings)
		{
			glDeleteLists(wing.getGLDisplayList(), 1);
		}

		cleanupDrawQuad();

		delete glslProgram;
		glslProgram = nullptr;
	}

	void AdvanceAnimation(void)
	{
		GLuint displayList{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			displayList = glGenLists(1);
		}
		else
		{
			displayList = wings.back().getGLDisplayList();
			wings.pop_back();
		}
		Wing const& wing{ wings.emplace_front(displayList,
			radiusCurve.getNextValue(), angleCurve.getNextValue(),
			deltaAngleCurve.getNextValue(), deltaZCurve.getNextValue(),
			rollCurve.getNextValue(), pitchCurve.getNextValue(), yawCurve.getNextValue(),
			Color{ redCurve.getNextValue(), greenCurve.getNextValue(), blueCurve.getNextValue() },
			Color::WHITE) };

		glNewList(displayList, GL_COMPILE);
		glVertexAttrib2f(radiusAngleAttribLocation, wing.getRadius(), wing.getAngle());
		glVertexAttrib3f(rollPitchYawAttribLocation, wing.getRoll(), wing.getPitch(), wing.getYaw());
		drawQuad();
		glEndList();
	}

	void DrawFrame(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (hasOpenGL(1, 1))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_POLYGON_OFFSET_LINE);
			GLfloat deltaZ{ 0 };
			GLfloat deltaAngle{ 0 };
			for (Wing const& wing : wings) {
				deltaZ += wing.getDeltaZ();
				deltaAngle += wing.getDeltaAngle();

				Color const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
				glCallList(wing.getGLDisplayList());
			}
			glDisable(GL_POLYGON_OFFSET_LINE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (Wing const& wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			Color const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
			glCallList(wing.getGLDisplayList());
		}

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
