#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>

#include "WingsViewGL3.h"

#include "CurveGenerator.h"
#include "WingGL3.h"

#include "WingTransformProgram.h"
#include "WingRenderProgram.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl3
{

	WingsViewGL3::WingsViewGL3(void)
	{
		GLint glMajorVersion{ 1 };
		GLint glMinorVersion{ 0 };
		glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

		/*
		 * Depth testing is a basic requirement when using a depth buffer.
		 */
		glEnable(GL_DEPTH_TEST);

		/*
		 * The body of each wing is rendered using polygon offset to reduce
		 * Z-fighting with the edge.
		 */
		glPolygonOffset(0.75, 2);
		glEnable(GL_POLYGON_OFFSET_FILL);

		/*
		 * The wing edges are rendered with smoothing enabled (antialiasing).
		 * This generates multiple fragments per line step with alpha values
		 * less than one, so blending is required for it to look correct.
		 */
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(1.0);

		/*
		 * Set up the pieces needed to render one single
		 * (untransformed, uncolored) wing.
		 */
		std::shared_ptr<VertexShader const> rotateMatrixShader{
			VertexShader::MakeRotateMatrixShader()
		};
		std::shared_ptr<VertexShader const> translateMatrixShader{
			VertexShader::MakeTranslateMatrixShader()
		};
		wingGeometry = std::make_shared<WingGeometry>();
		wingTransformProgram = std::make_unique<WingTransformProgram>(wingGeometry, rotateMatrixShader, translateMatrixShader);
		wingRenderProgram = std::make_unique<WingRenderProgram>(wingGeometry, rotateMatrixShader, translateMatrixShader);
	}

	void WingsViewGL3::AdvanceAnimation(void)
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

		std::shared_ptr<ArrayBuffer> vertexBuffer{ nullptr };
		std::shared_ptr<ArrayBuffer> colorBuffer{ nullptr };
		std::shared_ptr<ArrayBuffer> edgeColorBuffer{ nullptr };
		if (wings.empty() || wings.size() < numWings)
		{
			vertexBuffer = wingTransformProgram->CreateVertexBuffer();
			colorBuffer = wingTransformProgram->CreateColorBuffer();
			edgeColorBuffer = wingTransformProgram->CreateColorBuffer();
		}
		else
		{
			/*
			 * If a wing expires off the end of the list of wings, we can reuse
			 * the various buffers for the newly-created wing.  The old data
			 * will be overwritten.
			 */
			vertexBuffer = wings.back().getVertexBuffer();
			colorBuffer = wings.back().getColorBuffer();
			edgeColorBuffer = wings.back().getEdgeColorBuffer();
			wings.pop_back();
		}

		Wing const& newWing{ wings.emplace_front(deltaAngle, deltaZ, vertexBuffer, colorBuffer, edgeColorBuffer) };

		/*
		 * Run a vertex shader to transform the wing based on its current
		 * animation state, and capture the transformed geometry using transform
		 * feedback.
		 */
		wingTransformProgram->TransformWing(radius, angle,
			roll, pitch, yaw,
			red, green, blue,
			*vertexBuffer,
			*colorBuffer,
			*edgeColorBuffer);

		glFlush();
	}

	void WingsViewGL3::DrawFrame(void) const
	{
		wingRenderProgram->RenderWings(wings);

		glFlush();
	}

	void WingsViewGL3::Resize(GLsizei width, GLsizei height) const
	{
		Resize(0, 0, width, height);
	}

	void WingsViewGL3::Resize(GLint x, GLint y, GLsizei width, GLsizei height) const
	{
		/*
		 * The projection matrix transforms the fragment coordinates to the
		 * scale of [-1, 1], called "normalized device coordinates".
		 * The viewport transforms those into the coordinates expected by the
		 * windowing system.
		 */
		glViewport(x, y, width, height);

		wingRenderProgram->Resize(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	}

}
