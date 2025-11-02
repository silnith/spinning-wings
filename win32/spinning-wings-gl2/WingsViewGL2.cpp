#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>
#include <string>

#include <cassert>

#include "WingsViewGL2.h"

#include "Color.h"
#include "CurveGenerator.h"
#include "FragmentShader.h"
#include "GLInfo.h"
#include "Program.h"
#include "QuadRendererGL10.h"
#include "QuadRendererGL11.h"
#include "QuadRendererGL15.h"
#include "VertexShader.h"
#include "Wing.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl2
{

	WingsViewGL2::WingsViewGL2(silnith::wings::gl::GLInfo const & glInfo) :
		enablePolygonOffset{ glInfo.isAtLeastVersion(1, 1) }
	{
		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		if (enablePolygonOffset)
		{
			glPolygonOffset(-0.5, -2);
			glEnable(GL_POLYGON_OFFSET_LINE);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			glLineWidth(1.0);
		}

		glLoadIdentity();
		gluLookAt(0, 50, 50,
			0, 0, 13,
			0, 0, 1);

		if (glInfo.isAtLeastVersion(1, 5))
		{
			quadRenderer = std::make_unique<silnith::wings::gl::QuadRendererGL15>();
		}
		else if (glInfo.isAtLeastVersion(1, 1))
		{
			quadRenderer = std::make_unique<silnith::wings::gl::QuadRendererGL11>();
		}
		else
		{
			quadRenderer = std::make_unique<silnith::wings::gl::QuadRendererGL10>();
		}

		if (glInfo.isAtLeastVersion(2, 1))
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

			glslProgram = std::make_unique<Program>(
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
			);

			deltaZAttribLocation = glslProgram->getAttributeLocation("deltaZ"s);
			radiusAngleAttribLocation = glslProgram->getAttributeLocation("radiusAngle"s);
			rollPitchYawAttribLocation = glslProgram->getAttributeLocation("rollPitchYaw"s);

			glslProgram->useProgram();
		}
		else
		{
			// Does not have OpenGL 2.1, abort!
			assert(false);
			// This makes the quad renderer selection above irrelevant.  Heh heh.
		}
	}

	WingsViewGL2::~WingsViewGL2(void)
	{
		for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings)
		{
			GLuint const displayList{ wing.getGLDisplayList() };
			glDeleteLists(displayList, 1);
		}

		quadRenderer.release();

		glslProgram.release();
	}

	void WingsViewGL2::AdvanceAnimation(void)
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
		/// colors and rendering modes.
		/// </para>
		/// </remarks>
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
		glVertexAttrib2f(radiusAngleAttribLocation, radius, angle);
		glVertexAttrib3f(rollPitchYawAttribLocation, roll, pitch, yaw);
		quadRenderer->DrawQuad();
		glEndList();
	}

	void WingsViewGL2::DrawFrame(void) const
	{
		/*
		 * First, draw the solid wings using their solid color.
		 */
		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			Color<GLfloat> const& color{ wing.getColor() };
			glColor3f(color.getRed(), color.getGreen(), color.getBlue());
			glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
			glCallList(wing.getGLDisplayList());
		}

		if (enablePolygonOffset)
		{
			/*
			 * Second, draw the wing outlines using the outline color.
			 * The outlines have smoothing (antialiasing) enabled, which
			 * requires blending.
			 */
			glEnable(GL_BLEND);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			deltaZ = 0;
			deltaAngle = 0;
			for (std::deque<Wing<GLuint, GLfloat> >::const_reference wing : wings) {
				deltaZ += wing.getDeltaZ();
				deltaAngle += wing.getDeltaAngle();

				Color<GLfloat> const& edgeColor{ wing.getEdgeColor() };
				glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
				glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
				glCallList(wing.getGLDisplayList());
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_BLEND);
		}

		glFlush();
	}

	void WingsViewGL2::Resize(GLsizei width, GLsizei height) const
	{
		Resize(0, 0, width, height);
	}

	void WingsViewGL2::Resize(GLint x, GLint y, GLsizei width, GLsizei height) const
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

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(defaultLeft * xmult, defaultRight * xmult,
			defaultBottom * ymult, defaultTop * ymult,
			defaultNear, defaultFar);
		glMatrixMode(GL_MODELVIEW);
	}

}
