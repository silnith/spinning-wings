#include "WingsView.h"

#include <gl/GLU.h>

#include <cassert>
#include <deque>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

namespace silnith::wings::gl
{

	typedef std::deque<Wing<GLuint, GLfloat> > wing_list;

	size_t constexpr numWings{ 40 };

	GLuint glMajorVersion{ 1 };
	GLuint glMinorVersion{ 0 };

	GLuint wingDisplayList{ 0 };
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

	void InitializeOpenGLState(void)
	{
		GLubyte const* const glVersion{ glGetString(GL_VERSION) };

		assert(glVersion != NULL);

		ParseOpenGLVersion(glVersion);

		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

#if defined(GL_VERSION_1_1)
		if (hasOpenGL(1, 1))
		{
			glPolygonOffset(-0.5, -2);
			glEnable(GL_POLYGON_OFFSET_LINE);
		}
#endif

		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(1.0);

		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glLoadIdentity();
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);

		wingDisplayList = glGenLists(1);
		glNewList(wingDisplayList, GL_COMPILE);
		glBegin(GL_QUADS);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glEnd();
		glEndList();

		GLuint const wingLists{ glGenLists(numWings) };
		for (GLuint displayList{ wingLists }; displayList < wingLists + numWings; displayList++) {
			// This initializes the list of wings to hold the allocated GL display lists.
			// These display list identifiers are reused throughout the lifetime of the program.
			wings.emplace_back(displayList);
			glNewList(displayList, GL_COMPILE);
			glEndList();
		}
	}

	void CleanupOpenGLState(void)
	{
		for (wing_list::const_reference wing : wings)
		{
			GLuint const displayList{ wing.getGLDisplayList() };
			glDeleteLists(displayList, 1);
		}

		glDeleteLists(wingDisplayList, 1);
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

		GLuint const displayList{ wings.back().getGLDisplayList() };
		wings.pop_back();
		wings.emplace_front(displayList,
			radius, angle,
			deltaAngle, deltaZ,
			roll, pitch, yaw,
			Color<GLfloat>{ red, green, blue },
			Color<GLfloat>::WHITE);

		glNewList(displayList, GL_COMPILE);
		glPushMatrix();
		glRotatef(angle, 0, 0, 1);
		glTranslatef(radius, 0, 0);
		glRotatef(-yaw, 0, 0, 1);
		glRotatef(-pitch, 0, 1, 0);
		glRotatef(roll, 1, 0, 0);
		glCallList(wingDisplayList);
		glPopMatrix();
		glEndList();
	}

	void DrawFrame(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if defined(GL_VERSION_1_1)
		if (hasOpenGL(1, 1))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPushMatrix();
			for (wing_list::const_reference wing : wings) {
				glTranslatef(0, 0, wing.getDeltaZ());
				glRotatef(wing.getDeltaAngle(), 0, 0, 1);

				Color<GLfloat> const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glCallList(wing.getGLDisplayList());
			}
			glPopMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
#endif

		glPushMatrix();
		for (wing_list::const_reference wing : wings) {
			glTranslatef(0, 0, wing.getDeltaZ());
			glRotatef(wing.getDeltaAngle(), 0, 0, 1);

			Color<GLfloat> const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glCallList(wing.getGLDisplayList());
		}
		glPopMatrix();

		glFlush();
	}

	void Resize(GLsizei width, GLsizei height)
	{
		glViewport(0, 0, width, height);

		GLdouble constexpr defaultLeft{ -20 };
		GLdouble constexpr defaultRight{ 20 };
		GLdouble constexpr defaultBottom{ -20 };
		GLdouble constexpr defaultTop{ 20 };
		GLdouble constexpr defaultNear{ 35 };
		GLdouble constexpr defaultFar{ 105 };

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

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(defaultLeft * xmult, defaultRight * xmult,
			defaultBottom * ymult, defaultTop * ymult,
			defaultNear, defaultFar);
		glMatrixMode(GL_MODELVIEW);
	}

}
