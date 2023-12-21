#include "WingsViewGL2.h"

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

namespace silnith::wings::gl1_0
{

	// The GL display list for rendering a single quad.
	// This is used for the GL 1.0 rendering path.
	GLuint quadDisplayList{ 0 };

	void InitializeDrawQuad(void)
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
	}

	void DrawQuad(void)
	{
		glCallList(quadDisplayList);
	}

	void CleanupDrawQuad(void)
	{
		glDeleteLists(quadDisplayList, 1);
	}

}

namespace silnith::wings
{

	GLfloat const quadVertices[2 * 4]
	{
		1, 1,
		-1, 1,
		-1, -1,
		1, -1,
	};
	GLsizeiptr const quadVerticesSize{ sizeof(quadVertices) };
	GLuint const quadIndices[4]
	{
		0, 1, 2, 3,
	};
	GLsizeiptr const quadIndicesSize{ sizeof(quadIndices) };

	static_assert(quadVerticesSize == sizeof(GLfloat) * 2 * 4, "I do not know how sizeof works.");
	static_assert(quadIndicesSize == sizeof(GLuint) * 4, "I do not know how sizeof works.");

}

namespace silnith::wings::gl1_1
{

	void InitializeDrawQuad(void)
	{
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 */
		glVertexPointer(2, GL_FLOAT, 0, quadVertices);

		/*
		 * EnableClientState is executed immediately, it is not compiled into
		 * a display list.
		 */
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	void DrawQuad(void)
	{
		/*
		 * When creating a display list,
		 * DrawElements dereferences vertex pointers according to the client
		 * state, and the dereferenced vertices are compiled into the
		 * display list.
		 */
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, quadIndices);
	}

	void CleanupDrawQuad(void)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

}

namespace silnith::wings::gl1_5
{

	GLuint wingBufferObject{ 0 };
	GLuint wingIndicesBufferObject{ 0 };

	void InitializeDrawQuad(void)
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
		glVertexPointer(2, GL_FLOAT, 0, 0);

		glGenBuffers(1, &wingIndicesBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesSize, quadIndices, GL_STATIC_DRAW);

		glEnableClientState(GL_VERTEX_ARRAY);
	}

	void DrawQuad(void)
	{
		/*
		 * When compiled into a display list,
		 * DrawElements dereferences vertex pointers according to the client
		 * state, and the dereferenced vertices are compiled into the
		 * display list.
		 * 
		 * If there is an ELEMENT_ARRAY_BUFFER bound, then the last parameter
		 * to DrawElements is interpreted as an index/offset into the
		 * ELEMENT_ARRAY_BUFFER.
		 *
		 * Vertices indexed by the ELEMENT_ARRAY_BUFFER are interpreted according to
		 * the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	}

	void CleanupDrawQuad(void)
	{
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &wingIndicesBufferObject);
		glDeleteBuffers(1, &wingBufferObject);
	}

}

namespace silnith::wings::gl2
{

	typedef std::deque<Wing<GLuint, GLfloat> > wing_list;

	size_t const numWings{ 40 };

	GLuint glMajorVersion{ 1 };
	GLuint glMinorVersion{ 0 };

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

	void (*initializeDrawQuad)(void) { silnith::wings::gl1_0::InitializeDrawQuad };

	// The rendering path for rendering a single quad.
	// This points to the appropriate rendering path for the
	// active version of the GL.
	void (*drawQuad)(void) { silnith::wings::gl1_0::DrawQuad };

	void (*cleanupDrawQuad)(void) { silnith::wings::gl1_0::CleanupDrawQuad };

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
			glEnable(GL_POLYGON_OFFSET_LINE);
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
			initializeDrawQuad = silnith::wings::gl1_5::InitializeDrawQuad;
			drawQuad = silnith::wings::gl1_5::DrawQuad;
			cleanupDrawQuad = silnith::wings::gl1_5::CleanupDrawQuad;
		}
		else if (hasOpenGL(1, 1))
		{
			initializeDrawQuad = silnith::wings::gl1_1::InitializeDrawQuad;
			drawQuad = silnith::wings::gl1_1::DrawQuad;
			cleanupDrawQuad = silnith::wings::gl1_1::CleanupDrawQuad;
		}
		else
		{
			initializeDrawQuad = silnith::wings::gl1_0::InitializeDrawQuad;
			drawQuad = silnith::wings::gl1_0::DrawQuad;
			cleanupDrawQuad = silnith::wings::gl1_0::CleanupDrawQuad;
		}
		initializeDrawQuad();

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

			glslProgram = new Program{
				VertexShader{
					R"shaderText(#version 120

attribute vec2 deltaZ;
attribute vec2 radiusAngle;
attribute vec3 rollPitchYaw;

mat4 rotate(in float angle, in vec3 axis);
mat4 translate(in vec3 move);

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);

void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float deltaAngle = deltaZ[0];
    float deltaZ = deltaZ[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    mat4 deltaTransformation = translate(vec3(0, 0, deltaZ))
                               * rotate(deltaAngle, zAxis);
    mat4 wingTransformation = rotate(angle, zAxis)
                              * translate(vec3(radius, 0, 0))
                              * rotate(-yaw, zAxis)
                              * rotate(-pitch, yAxis)
                              * rotate(roll, xAxis);

    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * deltaTransformation * wingTransformation * gl_Vertex;
}
)shaderText",
					rotateMatrixFunctionDeclaration,
					translateMatrixFunctionDeclaration,
				},
				FragmentShader{
					R"shaderText(#version 120

void main() {
    gl_FragColor = gl_Color;
    gl_FragDepth = gl_FragCoord.z;
}
)shaderText",
				}
			};

			deltaZAttribLocation = glslProgram->getAttributeLocation("deltaZ");
			radiusAngleAttribLocation = glslProgram->getAttributeLocation("radiusAngle");
			rollPitchYawAttribLocation = glslProgram->getAttributeLocation("rollPitchYaw");

			glslProgram->useProgram();
		}
	}

	void CleanupOpenGLState(void)
	{
		for (wing_list::const_reference wing : wings)
		{
			glDeleteLists(wing.getGLDisplayList(), 1);
		}

		cleanupDrawQuad();

		delete glslProgram;
		glslProgram = nullptr;
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

		GLuint displayList{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			displayList = glGenLists(1);
		}
		else
		{
			// This block is simply so lastWing goes out of scope before the pop_back.
			{
				wing_list::const_reference lastWing{ wings.back() };
				displayList = lastWing.getGLDisplayList();
			}
			wings.pop_back();
		}

		wing_list::const_reference wing{ wings.emplace_front(displayList,
			radius, angle,
			deltaAngle, deltaZ,
			roll, pitch, yaw,
			Color<GLfloat>{ red, green, blue },
			Color<GLfloat>::WHITE) };

		glNewList(displayList, GL_COMPILE);
		glVertexAttrib2f(radiusAngleAttribLocation, radius, angle);
		glVertexAttrib3f(rollPitchYawAttribLocation, roll, pitch, yaw);
		drawQuad();
		glEndList();
	}

	void DrawFrame(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (hasOpenGL(1, 1))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			GLfloat deltaZ{ 0 };
			GLfloat deltaAngle{ 0 };
			for (wing_list::const_reference wing : wings) {
				deltaZ += wing.getDeltaZ();
				deltaAngle += wing.getDeltaAngle();

				Color<GLfloat> const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
				glCallList(wing.getGLDisplayList());
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (wing_list::const_reference wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			Color<GLfloat> const& color{ wing.getColor() };
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
	}

}
