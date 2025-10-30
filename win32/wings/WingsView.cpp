#include "WingsView.h"

#include <gl/GLU.h>

#include <cassert>
#include <deque>
#include <string>
#include <sstream>

#include "CurveGenerator.h"
#include "Wing.h"

namespace silnith::wings::gl
{

	WingsView::WingsView(GLInfo const& glInfo) :
		enablePolygonOffset{ glInfo.hasOpenGL(1, 1) },
		wingDisplayList{ glGenLists(1) }
	{
		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

#if defined(GL_VERSION_1_1)
		if (enablePolygonOffset)
		{
			glPolygonOffset(-0.5, -2);
			glEnable(GL_POLYGON_OFFSET_LINE);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glLineWidth(1.0);
		}
#endif

		glLoadIdentity();
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);

		glNewList(wingDisplayList, GL_COMPILE);
		glBegin(GL_QUADS);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glEnd();
		glEndList();
	}

	WingsView::~WingsView(void) noexcept
	{
		for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings)
		{
			GLuint const displayList{ wing.getGLDisplayList() };
			glDeleteLists(displayList, 1);
		}

		glDeleteLists(wingDisplayList, 1);
	}

	void WingsView::AdvanceAnimation(void)
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
			displayList = wings.back().getGLDisplayList();
			wings.pop_back();
		}

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

	void WingsView::DrawFrame(void) const
	{
		glPushMatrix();
		for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings) {
			glTranslatef(0, 0, wing.getDeltaZ());
			glRotatef(wing.getDeltaAngle(), 0, 0, 1);

			Color<GLfloat> const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glCallList(wing.getGLDisplayList());
		}
		glPopMatrix();

#if defined(GL_VERSION_1_1)
		if (enablePolygonOffset)
		{
			glEnable(GL_BLEND);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPushMatrix();
			for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings) {
				glTranslatef(0, 0, wing.getDeltaZ());
				glRotatef(wing.getDeltaAngle(), 0, 0, 1);

				Color<GLfloat> const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glCallList(wing.getGLDisplayList());
			}
			glPopMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_BLEND);
		}
#endif

		glFlush();
	}

	void WingsView::Resize(GLsizei width, GLsizei height) const
	{
		Resize(0, 0, width, height);
	}

	void WingsView::Resize(GLint x, GLint y, GLsizei width, GLsizei height) const
	{
		glViewport(x, y, width, height);

		GLdouble constexpr defaultLeft{ -20 };
		GLdouble constexpr defaultRight{ 20 };
		GLdouble constexpr defaultBottom{ -20 };
		GLdouble constexpr defaultTop{ 20 };
		GLdouble constexpr defaultNear{ 35 };
		GLdouble constexpr defaultFar{ 105 };

		/*
		 * These multipliers account for the aspect ratio of the window, so that
		 * the rendering does not distort.  The conditional is so that the larger
		 * number is always divided by the smaller, resulting in a multiplier no
		 * less than one.  This way, the viewing area is always expanded rather than
		 * contracted, and the expected viewing frustum is never clipped.
		 */
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
