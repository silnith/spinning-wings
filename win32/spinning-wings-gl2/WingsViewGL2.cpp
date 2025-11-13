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
#include "WingRendererGL10.h"
#include "WingRendererGL11.h"
#include "WingRendererGL15.h"
#include "VertexShader.h"
#include "Wing.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl2
{

	WingsViewGL2::WingsViewGL2(silnith::wings::gl::GLInfo const & glInfo) :
		enablePolygonOffset{ glInfo.isAtLeastVersion(1, 1) }
	{
		/*
		 * Depth testing is a basic requirement when using a depth buffer.
		 */
		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		if (enablePolygonOffset)
		{
			/*
			 * The edge of each wing is rendered using polygon offset to reduce
			 * Z-fighting with the body.
			 */
			glPolygonOffset(-0.75, -2);
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
		if (glInfo.isAtLeastVersion(1, 5))
		{
			wingRenderer = std::make_unique<silnith::wings::gl::WingRendererGL15>();
		}
		else if (glInfo.isAtLeastVersion(1, 1))
		{
			wingRenderer = std::make_unique<silnith::wings::gl::WingRendererGL11>();
		}
		else
		{
			wingRenderer = std::make_unique<silnith::wings::gl::WingRendererGL10>();
		}

		if (glInfo.isAtLeastVersion(2, 1))
		{
			/*
			 * This block scope is only here so the shaders go
			 * out of scope the moment they are no longer needed.
			 * 
			 * This could have been accomplished by inlining them into the
			 * call to create the GLSL program, but doing that makes that
			 * line of code quite long and difficult to understand.
			 */
			{
				/// <summary>
				/// The main vertex shader.  This has three input attributes,
				/// <c>deltaZ</c>, <c>radiusAngle</c>, and <c>rollPitchYaw</c>.
				/// It requires linking against the vertex shaders provided by
				/// <see cref="VertexShader::MakeRotateMatrixShader"/>,
				/// <see cref="VertexShader::MakeTranslateMatrixShader"/>, and
				/// <see cref="VertexShader::MakeScaleMatrixShader"/>.
				/// </summary>
				std::shared_ptr<VertexShader const> const mainVertexShader{
					std::make_shared<VertexShader const>(std::initializer_list<std::string>{
						Shader::versionDeclaration,
						R"shaderText(
attribute vec2 deltaZ;
attribute vec2 radiusAngle;
attribute vec3 rollPitchYaw;

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);
)shaderText"s,
						Shader::rotateMatrixFunctionDeclaration,
						Shader::translateMatrixFunctionDeclaration,
						Shader::scaleMatrixFunctionDeclaration,
						R"shaderText(
/*
 * The shader entry point.
 * Vertex shaders may read gl_Color, gl_SecondaryColor,
 * gl_Normal, gl_Vertex, gl_FogCoord,
 * and a bunch of texture coordinate stuff.
 * 
 * A vertex shader must write to gl_Position.
 * It may write to gl_PointSize and gl_ClipVertex.
 */
void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float deltaAngle = deltaZ[0];
    float dZ = deltaZ[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    /*
     * This implements the additional transformations
     * that were applied in the wing-rendering loop.
     * These are relative to the position of the wing in the list.
     * Specifically:
     * glTranslatef(0, 0, wing.getDeltaZ());
     * glRotatef(wing.getDeltaAngle(), 0, 0, 1);
     */
    mat4 deltaTransformation = translate(vec3(0, 0, dZ))
                               * rotate(deltaAngle, zAxis);
    /*
     * This implements the transformations that were handled by the
     * wing-specific display list in the OpenGL 1.0 version.
     * These are consistent for the lifetime of a wing.
     * Specifically:
     * glRotatef(angle, 0, 0, 1);
     * glTranslatef(radius, 0, 0);
     * glRotatef(-yaw, 0, 0, 1);
     * glRotatef(-pitch, 0, 1, 0);
     * glRotatef(roll, 1, 0, 0);
     */
    mat4 wingTransformation = rotate(angle, zAxis)
                              * translate(vec3(radius, 0, 0))
                              * rotate(-yaw, zAxis)
                              * rotate(-pitch, yAxis)
                              * rotate(roll, xAxis);

    /*
     * The OpenGL 1.0 version did not use face culling.
     * 
     * gl_Color is a built-in attribute.
     * gl_FrontColor and gl_BackColor are built-in varying variables.
     */
    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
    /*
     * A vertex shader must set the gl_Position variable.
     * gl_ModelViewProjectionMatrix is a built-in uniform.
     * gl_Vertex is a built-in attribute.
     */
    gl_Position = gl_ModelViewProjectionMatrix * deltaTransformation * wingTransformation * gl_Vertex;
}
)shaderText"s,
					})
				};
				std::shared_ptr<FragmentShader const> const mainFragmentShader{
					std::make_shared<FragmentShader const>(std::initializer_list<std::string>{
						Shader::versionDeclaration,
						R"shaderText(
/*
 * The shader entry point.
 * Fragment shaders may read the variables gl_FragCoord and gl_FrontFacing.
 */
void main() {
    /*
     * gl_Color is a built-in varying variable based on the gl_FrontColor
     * or gl_BackColor written in the vertex shader.
     * 
     * A fragment shader must write to gl_FragColor if the render target uses colors.
     * gl_FragDepth must be written if the depth buffer is enabled.
     */
    gl_FragColor = gl_Color;
    gl_FragDepth = gl_FragCoord.z;
}
)shaderText"s,
					})
				};
				/*
				 * There are actually four vertex shaders instead of just one.
				 * The one containing the main entry point is inlined above for
				 * clarity.  The other three provide implementations replacing
				 * fixed-function features without defining a main function, so
				 * the main shader can use them without being cluttered.
				 */
				std::initializer_list<std::shared_ptr<VertexShader const> > const vertexShaders{
					mainVertexShader,
					VertexShader::MakeRotateMatrixShader(),
					VertexShader::MakeTranslateMatrixShader(),
					VertexShader::MakeScaleMatrixShader(),
				};
				std::initializer_list<std::shared_ptr<FragmentShader const> > const fragmentShaders{
					mainFragmentShader,
				};
				glslProgram = std::make_unique<Program>(vertexShaders, fragmentShaders);
			}

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
	}

	void WingsViewGL2::AdvanceAnimation(void)
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
		 * animation state.  Since all transformations are applied in the
		 * vertex shader instead of using the modelview matrix, no matrix push
		 * or pop is necessary.
		 */
		glNewList(displayList, GL_COMPILE);
		glVertexAttrib2f(radiusAngleAttribLocation, radius, angle);
		glVertexAttrib3f(rollPitchYawAttribLocation, roll, pitch, yaw);
		wingRenderer->DrawWing();
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
			 * 
			 * In order to reduce Z-fighting, the depth test function is changed from the
			 * default "less than" to "less than or equal".  Also, writes to the depth
			 * buffer are disabled.  This way fragments generated for the lines will be
			 * discarded if the line is behind an existing polygon, but drawn otherwise.
			 * And corners where lines adjoin will allow overlapping partial fragments to
			 * blend together rather than displace each other.
			 */
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);
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
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
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
