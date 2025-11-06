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

	typedef std::deque<Wing<GLfloat> > wing_list;

	size_t constexpr numWings{ 40 };

	GLint glMajorVersion{ 1 };
	GLint glMinorVersion{ 0 };

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

	std::unique_ptr<WingTransformProgram> wingTransformProgram{ nullptr };
	std::unique_ptr<WingRenderProgram> wingRenderProgram{ nullptr };

	void InitializeOpenGLState(void)
	{
		glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

		/*
		 * Depth testing is a basic requirement when using a depth buffer.
		 */
		glEnable(GL_DEPTH_TEST);

		/*
		 * The body of each wing is rendered using polygon offset to prevent
		 * Z-fighting.
		 */
		glPolygonOffset(0.5, 2);
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
		wingTransformProgram = std::make_unique<WingTransformProgram>();
		wingRenderProgram = std::make_unique<WingRenderProgram>();
	}

	void CleanupOpenGLState(void)
	{
		wingRenderProgram.reset();
		wingTransformProgram.reset();
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

		if (wings.empty() || wings.size() < numWings)
		{
			wings.emplace_front(deltaAngle, deltaZ);
		}
		else
		{
			std::shared_ptr<TransformedVertexBuffer> vertexBuffer{ nullptr };
			std::shared_ptr<TransformedColorBuffer> colorBuffer{ nullptr };
			std::shared_ptr<TransformedColorBuffer> edgeColorBuffer{ nullptr };
			// This block is simply so lastWing goes out of scope before the pop_back.
			{
				wing_list::const_reference lastWing{ wings.back() };
				vertexBuffer = lastWing.getVertexBuffer();
				colorBuffer = lastWing.getColorBuffer();
				edgeColorBuffer = lastWing.getEdgeColorBuffer();
			}
			wings.pop_back();
			wings.emplace_front(vertexBuffer, colorBuffer, edgeColorBuffer, deltaAngle, deltaZ);
		}

		Wing<GLfloat> const& newWing{ wings.front() };

		wingTransformProgram->TransformWing(radius, angle,
			roll, pitch, yaw,
			red, green, blue,
			*newWing.getVertexBuffer(),
			*newWing.getColorBuffer(),
			*newWing.getEdgeColorBuffer());
	}

	void DrawFrame(void)
	{
		wingRenderProgram->RenderWings(wings);

		glFlush();
	}

	void Resize(GLsizei width, GLsizei height)
	{
		Resize(0, 0, width, height);
	}

	void Resize(GLint x, GLint y, GLsizei width, GLsizei height)
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
