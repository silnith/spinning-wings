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

	Program glslProgram{};

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
	void (*drawQuad) (void) { DrawQuadGL1_0 };

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
	}

	GLfloat const quadVertices[12]
	{
		1, 1, 0,
		-1, 1, 0,
		-1, -1, 0,
		1, -1, 0,
	};
	GLuint const quadIndices[4]
	{
		0, 1, 2, 3,
	};

	void DrawQuadGL1_1(void)
	{
		//glDrawArrays(GL_QUADS, 0, 4);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, quadIndices);
	}

	void InitializeDrawQuadGL1_1(void)
	{
		glVertexPointer(3, GL_FLOAT, 0, quadVertices);

		glEnableClientState(GL_VERTEX_ARRAY);

		drawQuad = DrawQuadGL1_1;
	}

	void DrawQuadGL1_5(void)
	{
		//glDrawArrays(GL_QUADS, 0, 4);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	}

	GLuint wingBufferObject{ 0 };
	GLuint wingIndicesBufferObject{ 0 };

	void InitializeDrawQuadGL1_5(void)
	{
		GLsizeiptr const quadVerticesSize{ sizeof(GLfloat) * 12 };
		assert(quadVerticesSize == sizeof(quadVertices));
		GLsizeiptr const quadIndicesSize{ sizeof(GLuint) * 4 };
		assert(quadIndicesSize == sizeof(quadIndices));
		
		glGenBuffers(1, &wingBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, wingBufferObject);
		glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glGenBuffers(1, &wingIndicesBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesSize, quadIndices, GL_STATIC_DRAW);

		glEnableClientState(GL_VERTEX_ARRAY);

		drawQuad = DrawQuadGL1_5;
	}

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

		// Initialize draw quad function pointer.
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

		if (hasOpenGL(2, 0))
		{
			std::vector<std::string> vertexSources{
				"#version 110",
				"",
				"void main() {",
				"    gl_FrontColor = gl_Color;",
				"    gl_BackColor = gl_Color;",
				"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;",
				"}",
			};

			std::vector<std::string> fragmentSources{
				"#version 110",
				"",
				"void main() {",
				"    gl_FragColor = gl_Color;",
				"    gl_FragDepth = gl_FragCoord.z;",
				"}",
			};

			glslProgram = { VertexShader{ vertexSources }, FragmentShader{ fragmentSources } };

			glslProgram.useProgram();
		}
	}

	void AdvanceAnimation(void)
	{
		GLuint displayList{ 0 };
		if (wings.size() < numWings) {}
		else
		{
			wings.pop_back();
		}
		Wing const& wing{ wings.emplace_front(displayList,
			radiusCurve.getNextValue(), angleCurve.getNextValue(),
			deltaAngleCurve.getNextValue(), deltaZCurve.getNextValue(),
			rollCurve.getNextValue(), pitchCurve.getNextValue(), yawCurve.getNextValue(),
			Color{ redCurve.getNextValue(), greenCurve.getNextValue(), blueCurve.getNextValue() },
			Color::WHITE) };
	}

	void DrawFrame(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (hasOpenGL(1, 1))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPushMatrix();
			for (Wing const& wing : wings) {
				glTranslatef(0, 0, wing.getDeltaZ());
				glRotatef(wing.getDeltaAngle(), 0, 0, 1);

				Color const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glPushMatrix();
				glRotatef(wing.getAngle(), 0, 0, 1);
				glTranslatef(wing.getRadius(), 0, 0);
				glRotatef(-(wing.getYaw()), 0, 0, 1);
				glRotatef(-(wing.getPitch()), 0, 1, 0);
				glRotatef(wing.getRoll(), 1, 0, 0);
				drawQuad();
				glPopMatrix();
			}
			glPopMatrix();
			glDisable(GL_POLYGON_OFFSET_LINE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glPushMatrix();
		for (Wing const& wing : wings) {
			glTranslatef(0, 0, wing.getDeltaZ());
			glRotatef(wing.getDeltaAngle(), 0, 0, 1);

			Color const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glPushMatrix();
			glRotatef(wing.getAngle(), 0, 0, 1);
			glTranslatef(wing.getRadius(), 0, 0);
			glRotatef(-(wing.getYaw()), 0, 0, 1);
			glRotatef(-(wing.getPitch()), 0, 1, 0);
			glRotatef(wing.getRoll(), 1, 0, 0);
			drawQuad();
			glPopMatrix();
		}
		glPopMatrix();

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