#include "WingsViewGL2.h"

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

#include <map>

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

	void CheckForGLErrors(std::map<GLenum, std::string> const& errors)
	{
		GLenum error{ glGetError() };
		//assert(error == GL_NO_ERROR);
		while (error != GL_NO_ERROR)
		{
			std::map<GLenum, std::string>::const_iterator const errorLookup{ errors.find(error) };
			if (errorLookup == errors.cend())
			{
			}
			else
			{
				throw new std::runtime_error{ errorLookup->second };
			}
			//switch (error)
			//{
			//case GL_INVALID_ENUM: break;
			//case GL_INVALID_VALUE: break;
			//case GL_INVALID_OPERATION: break;
			//case GL_STACK_OVERFLOW: break;
			//case GL_STACK_UNDERFLOW: break;
			//case GL_OUT_OF_MEMORY: break;
			//case GL_TABLE_TOO_LARGE: break;
			//default: break;
			//}
			error = glGetError();
		}
	}

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
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Called list 0." },
		});
#endif
	}

	// The rendering path for rendering a single quad.
	// This points to the appropriate rendering path for the
	// active version of the GL.
	void (*drawQuad)(void) { DrawQuadGL1_0 };

	void CleanupDrawQuadGL1_0(void)
	{
		glDeleteLists(quadDisplayList, 1);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Range is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
	}

	void (*cleanupDrawQuad)(void) { CleanupDrawQuadGL1_0 };

	void InitializeDrawQuadGL1_0(void)
	{
		quadDisplayList = glGenLists(1);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Range is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glNewList(quadDisplayList, GL_COMPILE);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "List is 0." },
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or display list is already being defined." },
			{ GL_OUT_OF_MEMORY, "Insufficient memory to compile display list." },
		});
#endif
		glBegin(GL_QUADS);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Invalid nesting of glBegin or glEnd, or invalid command between glBegin and glEnd." },
		});
#endif
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glEnd();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Invalid nesting of glBegin or glEnd, or invalid command between glBegin and glEnd." },
		});
#endif
		glEndList();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "List is 0." },
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or display list is already being defined." },
			{ GL_OUT_OF_MEMORY, "Insufficient memory to compile display list." },
		});
#endif

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
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
		});
#endif

		/*
		 * Elements are interpreted according to the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, quadIndices);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_VALUE, "Count is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or buffer object name something something, or buffer is mapped." },
		});
#endif

		glDisableClientState(GL_VERTEX_ARRAY);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
		});
#endif
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
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid type." },
			{ GL_INVALID_VALUE, "Size is not 2, 3, or 4, or stride is negative." },
		});
#endif

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
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
		});
#endif

		/*
		 * If there is an ELEMENT_ARRAY_BUFFER bound, then the last parameter
		 * to DrawElements is interpreted as an index/offset into the
		 * ELEMENT_ARRAY_BUFFER.
		 *
		 * Elements of the ELEMENT_ARRAY_BUFFER are interpreted according to
		 * the current VertexPointer.
		 */
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_VALUE, "Count is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or buffer object name something something, or buffer is mapped." },
		});
#endif

		glDisableClientState(GL_VERTEX_ARRAY);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
		});
#endif
	}

	GLuint wingBufferObject{ 0 };
	GLuint wingIndicesBufferObject{ 0 };

	void CleanupDrawQuadGL1_5(void)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		glDeleteBuffers(1, &wingIndicesBufferObject);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Value is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glDeleteBuffers(1, &wingBufferObject);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Value is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
	}

	void InitializeDrawQuadGL1_5(void)
	{
		glGenBuffers(1, &wingBufferObject);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Value is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glBindBuffer(GL_ARRAY_BUFFER, wingBufferObject);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target, or usage." },
			{ GL_INVALID_VALUE, "Value is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or using reserved buffer object name 0." },
			{ GL_OUT_OF_MEMORY, "Out of memory." },
		});
#endif
		/*
		 * Specifies vertex data to be used by subsequent calls to
		 * DrawArrays and DrawElements.
		 *
		 * If there is an ARRAY_BUFFER bound, then the last parameter is interpreted
		 * as an index/offset into the ARRAY_BUFFER.
		 */
		glVertexPointer(3, GL_FLOAT, 0, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid type." },
			{ GL_INVALID_VALUE, "Size is not 2, 3, or 4, or stride is negative." },
		});
#endif

		glGenBuffers(1, &wingIndicesBufferObject);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Value is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndicesBufferObject);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesSize, quadIndices, GL_STATIC_DRAW);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target, or usage." },
			{ GL_INVALID_VALUE, "Value is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or using reserved buffer object name 0." },
			{ GL_OUT_OF_MEMORY, "Out of memory." },
		});
#endif

		drawQuad = DrawQuadGL1_5;
		cleanupDrawQuad = CleanupDrawQuadGL1_5;
	}

	void InitializeOpenGLState(void)
	{
		GLubyte const* const glVendor{ glGetString(GL_VENDOR) };
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid name." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		GLubyte const* const glRenderer{ glGetString(GL_RENDERER) };
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid name." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		GLubyte const* const glVersion{ glGetString(GL_VERSION) };
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid name." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		GLubyte const* const glExtensions{ glGetString(GL_EXTENSIONS) };
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid name." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		assert(glVendor != NULL);
		assert(glRenderer != NULL);
		assert(glVersion != NULL);
		assert(glExtensions != NULL);

		ParseOpenGLVersion(glVersion);

		glEnable(GL_DEPTH_TEST);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid value." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		if (hasOpenGL(1, 1))
		{
			glPolygonOffset(-0.5, -2);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
		}

		glEnable(GL_LINE_SMOOTH);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid value." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glLineWidth(1.0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Width is less than or equal to 0." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		glEnable(GL_POLYGON_SMOOTH);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid value." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target or mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target or mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target or mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid target or mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		glLoadIdentity();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

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

			glslProgram = new Program{ VertexShader{ vertexSources }, FragmentShader{ fragmentSources } };

			glslProgram->useProgram();
		}
	}

	void CleanupOpenGLState(void)
	{
		for (Wing const& wing : wings)
		{
			glDeleteLists(wing.getGLDisplayList(), 1);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_VALUE, "Range is negative." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
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
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_VALUE, "Range is negative." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
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
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "List is 0." },
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or display list is already being defined." },
			{ GL_OUT_OF_MEMORY, "Insufficient memory to compile display list." },
		});
#endif
		glPushMatrix();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_STACK_OVERFLOW, "Matrix stack is full." },
			{ GL_STACK_UNDERFLOW, "Matrix stack is empty." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glRotatef(wing.getAngle(), 0, 0, 1);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glTranslatef(wing.getRadius(), 0, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glRotatef(-(wing.getYaw()), 0, 0, 1);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glRotatef(-(wing.getPitch()), 0, 1, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glRotatef(wing.getRoll(), 1, 0, 0);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		drawQuad();
		glPopMatrix();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_STACK_OVERFLOW, "Matrix stack is full." },
			{ GL_STACK_UNDERFLOW, "Matrix stack is empty." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glEndList();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "List is 0." },
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd, or display list is already being defined." },
			{ GL_OUT_OF_MEMORY, "Insufficient memory to compile display list." },
		});
#endif
	}

	void DrawFrame(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Invalid bit." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		if (hasOpenGL(1, 1))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_ENUM, "Invalid face or mode." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
			glEnable(GL_POLYGON_OFFSET_LINE);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_ENUM, "Invalid value." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
			glPushMatrix();
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_STACK_OVERFLOW, "Matrix stack is full." },
				{ GL_STACK_UNDERFLOW, "Matrix stack is empty." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
			for (Wing const& wing : wings) {
				glTranslatef(0, 0, wing.getDeltaZ());
#ifndef NDEBUG
				CheckForGLErrors(std::map<GLenum, std::string>
				{
					{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
				});
#endif
				glRotatef(wing.getDeltaAngle(), 0, 0, 1);
#ifndef NDEBUG
				CheckForGLErrors(std::map<GLenum, std::string>
				{
					{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
				});
#endif

				Color const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glCallList(wing.getGLDisplayList());
#ifndef NDEBUG
				CheckForGLErrors(std::map<GLenum, std::string>
				{
					{ GL_INVALID_VALUE, "Called list 0." },
				});
#endif
			}
			glPopMatrix();
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_STACK_OVERFLOW, "Matrix stack is full." },
				{ GL_STACK_UNDERFLOW, "Matrix stack is empty." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
			glDisable(GL_POLYGON_OFFSET_LINE);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_ENUM, "Invalid value." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_ENUM, "Invalid face or mode." },
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
		}

		glPushMatrix();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_STACK_OVERFLOW, "Matrix stack is full." },
			{ GL_STACK_UNDERFLOW, "Matrix stack is empty." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		for (Wing const& wing : wings) {
			glTranslatef(0, 0, wing.getDeltaZ());
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif
			glRotatef(wing.getDeltaAngle(), 0, 0, 1);
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
			});
#endif

			Color const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glCallList(wing.getGLDisplayList());
#ifndef NDEBUG
			CheckForGLErrors(std::map<GLenum, std::string>
			{
				{ GL_INVALID_VALUE, "Called list 0." },
			});
#endif
		}
		glPopMatrix();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_STACK_OVERFLOW, "Matrix stack is full." },
			{ GL_STACK_UNDERFLOW, "Matrix stack is empty." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif

		glFlush();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
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
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Width or height is negative." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glMatrixMode(GL_PROJECTION);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glLoadIdentity();
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glOrtho(static_cast<GLdouble>(-20) * xmult, static_cast<GLdouble>(20) * xmult,
			static_cast<GLdouble>(-20) * ymult, static_cast<GLdouble>(20) * ymult,
			static_cast<GLdouble>(35), static_cast<GLdouble>(105));
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_VALUE, "Left = right, or bottom = top, or near = far." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
		glMatrixMode(GL_MODELVIEW);
#ifndef NDEBUG
		CheckForGLErrors(std::map<GLenum, std::string>
		{
			{ GL_INVALID_ENUM, "Invalid mode." },
			{ GL_INVALID_OPERATION, "Called between glBegin and glEnd." },
		});
#endif
	}

}
