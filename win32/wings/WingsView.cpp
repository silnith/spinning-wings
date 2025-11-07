#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <deque>
#include <string>
#include <sstream>

#include <cassert>

#include "WingsView.h"

#include "Color.h"
#include "CurveGenerator.h"
#include "Wing.h"

namespace silnith::wings::gl
{

	WingsView::WingsView(GLInfo const& glInfo) :
		enablePolygonOffset{ glInfo.isAtLeastVersion(1, 1) },
		wingDisplayList{ glGenLists(1) }
	{
		/*
		 * Depth testing is a basic requirement when using a depth buffer.
		 */
		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		/*
		 * The polygon offset functionality was introduced with OpenGL 1.1.
		 */
#if defined(GL_VERSION_1_1)
		if (enablePolygonOffset)
		{
			/*
			 * The edge of each wing is rendered using polygon offset to prevent
			 * Z-fighting.
			 */
			glPolygonOffset(-0.5, -2);
			glEnable(GL_POLYGON_OFFSET_LINE);

			/*
			 * The wing edges are rendered with smoothing enabled (antialiasing).
			 * This generates multiple fragments per line step with alpha values
			 * less than one, so blending is required for it to look correct.
			 */
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glLineWidth(1.0);
		}
#endif

		/*
		 * Set up the initial camera position.
		 */
		glLoadIdentity();
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);

		/*
		 * Set up the pieces needed to render one single
		 * (untransformed, uncolored) wing.
		 */
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
		/*
		 * Get the next updated values for all the parameters that define how
		 * a wing moves.
		 */
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

		/// <summary>
		/// The display list for the new wing.
		/// </summary>
		/// <remarks>
		/// <para>
		/// This display list will be compiled to apply the transformations
		/// that are specific to the wing.  This includes the orientation of
		/// the wing (roll, pitch, yaw) as well as its radius and angle from
		/// the central axis.  It does not include the "delta" parameters,
		/// because those are relative between successive wings and so must
		/// be applied by the core rendering loop.  The colors are also not
		/// included because the display list will be used twice with different
		/// colors and rendering modes (assuming GL 1.1 is supported).
		/// </para>
		/// </remarks>
		GLuint displayList{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			displayList = glGenLists(1);
		}
		else
		{
			/*
			 * If a wing expires off the end of the list of wings, we can reuse
			 * the display list identifier for the newly-created wing.  The old
			 * data will be overwritten.
			 */
			displayList = wings.back().getGLDisplayList();
			wings.pop_back();
		}

		wings.emplace_front(displayList,
			radius, angle,
			deltaAngle, deltaZ,
			roll, pitch, yaw,
			Color<GLfloat>{ red, green, blue },
			Color<GLfloat>::WHITE);

		/*
		 * Create a display list that transforms the wing based on its current
		 * animation state.  In order to preserve the state of the modelview
		 * matrix, the current matrix is pushed and restored.  That makes execution
		 * of this display list idempotent (in terms of rendering state).
		 */
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
		/*
		 * First, draw the solid wings using their solid color.
		 */
		glPushMatrix();
		for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings) {
			/*
			 * Allow the delta transformations to accumulate as we go through the list
			 * of wings.
			 */
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
			/*
			 * Second, draw the wing outlines using the outline color.
			 * The outlines have smoothing (antialiasing) enabled, which
			 * requires blending.
			 */
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
		/*
		 * The projection matrix transforms the fragment coordinates to the
		 * scale of [-1, 1], called "normalized device coordinates".
		 * The viewport transforms those into the coordinates expected by the
		 * windowing system.
		 */
		glViewport(x, y, width, height);

		/*
		 * The view frustum was hand-selected to match the parameters to the
		 * curve generators and the initial gluLookAt().
		 */
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

		/*
		 * Set up the projection matrix.
		 * The projection matrix is only used for the viewing frustum.
		 * Things like camera position belong in the modelview matrix.
		 */
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(defaultLeft * xmult, defaultRight * xmult,
			defaultBottom * ymult, defaultTop * ymult,
			defaultNear, defaultFar);
		glMatrixMode(GL_MODELVIEW);
	}

}
