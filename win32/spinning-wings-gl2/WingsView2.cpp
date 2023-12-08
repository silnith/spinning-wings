#include "WingsView2.h"

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

namespace silnith {

	typedef std::deque<Wing> wing_list;

	size_t const numWings{ 40 };

	GLuint glMajorVersion{ 1 };
	GLuint glMinorVersion{ 0 };

	GLuint wingDisplayList{ 0 };
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

	void FetchShaderError(GLuint const shader)
	{
		GLint messageSize{ 0 };
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &messageSize);
		GLchar* log = new GLchar[messageSize];
		glGetShaderInfoLog(shader, messageSize, NULL, log);
		// TODO: print it out somehow
		delete[] log;
	}

	void CompileShader(GLuint const shader, std::string const& source)
	{
		GLchar const* cSource{ source.c_str() };
		glShaderSource(shader, 1, &cSource, NULL);
		glCompileShader(shader);

		GLint compilationSuccess{ 0 };
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccess);
		switch (compilationSuccess)
		{
		case GL_TRUE:
			break;
		case GL_FALSE:
			FetchShaderError(shader);
			break;
		default:
			assert(false);
			break;
		}
	}

	void FetchProgramError(GLuint const program)
	{
		GLint messageSize{ 0 };
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &messageSize);
		GLchar* log = new GLchar[messageSize];
		glGetProgramInfoLog(program, messageSize, NULL, log);
		// TODO: print it out somehow
		delete[] log;
	}

	void InitializeOpenGLState2(void)
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

		GLuint const wingLists{ glGenLists(numWings) };
		for (GLuint displayList{ wingLists }; displayList < wingLists + numWings; displayList++) {
			// This initializes the list of wings to hold the allocated GL display lists.
			// These display list identifiers are reused throughout the lifetime of the program.
			wings.emplace_back(displayList);
		}

		std::string const vertexShaderSource{
			"#version 110\n"\
			"\n"\
			"void main() {\n"\
			"    gl_FrontColor = gl_Color;\n"\
			"    gl_BackColor = gl_Color;\n"\
			"    gl_Position = gl_Vertex;\n"\
			"}\n"
		};
		std::string const fragmentShaderSource{
			"#version 110\n"\
			"\n"\
			"void main() {\n"\
			"    //gl_FragColor = gl_Color;\n"\
			"    gl_FragColor.r = 1.0;\n"\
			"    gl_FragColor.g = 0.0;\n"\
			"    gl_FragColor.b = 0.0;\n"\
			"    gl_FragDepth = gl_FragCoord.z;\n"\
			"}\n"
		};
		GLuint vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
		GLuint fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };

		CompileShader(vertexShader, vertexShaderSource);
		CompileShader(fragmentShader, fragmentShaderSource);

		GLuint program{ glCreateProgram() };
		//glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		//glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLint linkStatus{ 0 };
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		switch (linkStatus)
		{
		case GL_TRUE:
			break;
		case GL_FALSE:
			FetchProgramError(program);
			break;
		default:
			assert(false);
			break;
		}

		glUseProgram(program);
	}

	void Temp(void)
	{
		GLint success{ 0 };
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		std::string const vertexShaderSource{ "#version 110\n"\
			"attribute vec4 vertexPosition;\n"\
			"in vec4 vertexColor;\n"\
			"in uint wingIndex;\n"\
			"in float deltaAngle;\n"\
			"in float deltaZ;\n"\
			"in float radius;\n"\
			"in float angle;\n"\
			"in float roll;\n"\
			"in float pitch;\n"\
			"in float yaw;\n"\
			"smooth out vec4 color;\n"\
			"uniform mat4 modelviewMatrix;\n"\
			"void main() {\n"\
			"gl_Position = vertexPosition;\n"\
			"color = vertexColor;\n"\
			"}\n" };
		GLchar const* vertexShaderSourcePointer{ vertexShaderSource.c_str() };

		glShaderSource(vertexShader, 1, &vertexShaderSourcePointer, NULL);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			// glGetShaderInfoLog();
			char logOutput[1024];
			GLsizei logSize{};
			glGetShaderInfoLog(vertexShader, 1024, &logSize, logOutput);
			assert(0 == 1);
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string const fragmentShaderSource = "#version 110\n"\
			"smooth in vec4 color;\n"\
			"out vec4 fragmentColor;\n"\
			"void main() {\n"\
			"fragmentColor = color;\n"\
			"}\n";
		GLchar const* fragmentShaderSourcePointer{ fragmentShaderSource.c_str() };

		glShaderSource(fragmentShader, 1, &fragmentShaderSourcePointer, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			// glGetShaderInfoLog();
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glBindAttribLocation(program, 0, "vertexPosition");
		glBindAttribLocation(program, 1, "vertexColor");
		glBindAttribLocation(program, 2, "wingIndex");
		glBindAttribLocation(program, 3, "deltaAngle");
		glBindAttribLocation(program, 4, "deltaZ");
		glBindAttribLocation(program, 5, "radius");
		glBindAttribLocation(program, 6, "angle");
		glBindAttribLocation(program, 7, "roll");
		glBindAttribLocation(program, 8, "pitch");
		glBindAttribLocation(program, 9, "yaw");

		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (success == GL_FALSE) {
			// glGetProgramInfoLog()
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glValidateProgram(program);

		GLint uniformLocation{ glGetUniformLocation(program, "modelviewMatrix") };

		glUseProgram(program);

		// do stuff
		GLfloat modelviewMatrix[16]{};
		glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, modelviewMatrix);

		glUseProgram(NULL);

		glDeleteProgram(program);

		GLuint vao[1];
		glGenVertexArrays(1, vao);
		glBindVertexArray(vao[0]);

		//GLuint buffer[1];
		//glGenBuffers(1, buffer);
		//glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
		// glBufferData, glMapBuffer, glCopyBuffer
		// glVertexAttribPointer();

		glVertexPointer(3, GL_FLOAT, 0, nullptr);
		glEnableClientState(GL_VERTEX_ARRAY);

		//glDrawArrays(GL_QUADS, 0, 4);
		//glDrawElements

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, 0);

		GLuint buffers[40]{};
		glGenBuffers(40, buffers);
	}

	void InitializeOpenGLBuffers(void)
	{
		GLuint singleQuadVertexBuffer;
		glGenBuffers(1, &singleQuadVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, singleQuadVertexBuffer);
		GLfloat const quad[12]{
			1, 1, 0,
			-1, 1, 0,
			-1, -1, 0,
			1, -1, 0,
		};
		GLsizeiptr const quadSize{ sizeof(GLfloat) * 4 * 3 };
		glBufferData(GL_ARRAY_BUFFER, quadSize, quad, GL_STATIC_DRAW);
	}

	void AdvanceAnimation2(void)
	{
		GLuint const displayList{ wings.back().getGLDisplayList() };
		wings.pop_back();
		silnith::Wing const& wing{ wings.emplace_front(displayList,
			radiusCurve.getNextValue(), angleCurve.getNextValue(),
			deltaAngleCurve.getNextValue(), deltaZCurve.getNextValue(),
			rollCurve.getNextValue(), pitchCurve.getNextValue(), yawCurve.getNextValue(),
			silnith::Color{ redCurve.getNextValue(), greenCurve.getNextValue(), blueCurve.getNextValue() },
			silnith::Color::WHITE) };
	}

	void DrawFrame2(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static GLfloat const quadVertices[12]
		{
			1, 1, 0,
			-1, 1, 0,
			-1, -1, 0,
			1, -1, 0,
		};
		glVertexPointer(3, GL_FLOAT, 0, quadVertices);

		static GLuint const quadIndices[4]
		{
			0, 1, 2, 3,
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		if (hasOpenGL(1, 1))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPushMatrix();
			for (silnith::Wing const& wing : wings) {
				glTranslatef(0, 0, wing.getDeltaZ());
				glRotatef(wing.getDeltaAngle(), 0, 0, 1);

				silnith::Color const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glPushMatrix();
				glRotatef(wing.getAngle(), 0, 0, 1);
				glTranslatef(wing.getRadius(), 0, 0);
				glRotatef(-(wing.getYaw()), 0, 0, 1);
				glRotatef(-(wing.getPitch()), 0, 1, 0);
				glRotatef(wing.getRoll(), 1, 0, 0);
				//glDrawArrays(GL_QUADS, 0, 4);
				glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, quadIndices);
				glPopMatrix();
			}
			glPopMatrix();
			glDisable(GL_POLYGON_OFFSET_LINE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glPushMatrix();
		for (silnith::Wing const& wing : wings) {
			glTranslatef(0, 0, wing.getDeltaZ());
			glRotatef(wing.getDeltaAngle(), 0, 0, 1);

			silnith::Color const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glPushMatrix();
			glRotatef(wing.getAngle(), 0, 0, 1);
			glTranslatef(wing.getRadius(), 0, 0);
			glRotatef(-(wing.getYaw()), 0, 0, 1);
			glRotatef(-(wing.getPitch()), 0, 1, 0);
			glRotatef(wing.getRoll(), 1, 0, 0);
			//glDrawArrays(GL_QUADS, 0, 4);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, quadIndices);
			glPopMatrix();
		}
		glPopMatrix();
		glDisableClientState(GL_VERTEX_ARRAY);

		glFlush();
	}

	void Resize2(GLsizei width, GLsizei height)
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

};
