#include "WingsViewGL4.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cassert>
#include <deque>
#include <memory>
#include <sstream>

#include "CurveGenerator.h"
#include "WingGL4.h"

#include "WingGeometry.h"

#include "ArrayBuffer.h"
#include "ElementArrayBuffer.h"
#include "ModelViewProjectionUniformBuffer.h"
#include "WingTransformFeedback.h"

#include "WingTransformProgram.h"
#include "WingRenderProgram.h"

#include "RotateVertexShader.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl4
{

	std::size_t constexpr numWings{ 40 };

	// TODO: Investigate glObjectLabel
	GLint glMajorVersion{ 1 };
	GLint glMinorVersion{ 0 };

	std::deque<Wing> wings{};

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
		std::shared_ptr<WingGeometry const> wingGeometry{ std::make_shared<WingGeometry const>() };

		std::shared_ptr<RotateVertexShader const> rotateMatrixShader{ std::make_shared<RotateVertexShader const>() };
		std::shared_ptr<VertexShader const> translateMatrixShader{
			VertexShader::MakeTranslateMatrixShader()
		};
		std::shared_ptr<VertexShader const> scaleMatrixShader{
			VertexShader::MakeScaleMatrixShader()
		};
		wingTransformProgram = std::make_unique<WingTransformProgram>(wingGeometry, rotateMatrixShader, translateMatrixShader);

		wingRenderProgram = std::make_unique<WingRenderProgram>(wingGeometry, rotateMatrixShader, translateMatrixShader);

		glReleaseShaderCompiler();
	}

	void CleanupOpenGLState(void)
	{
		wings.clear();

		wingTransformProgram = nullptr;
		wingRenderProgram = nullptr;
	}

	void AdvanceAnimation(void)
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

		std::shared_ptr<WingTransformFeedback const> wingTransformFeedbackObject{ nullptr };
		if (wings.empty() || wings.size() < numWings)
		{
			wingTransformFeedbackObject = wingTransformProgram->CreateTransformFeedback();
		}
		else
		{
			/*
			 * If a wing expires off the end of the list of wings, we can reuse
			 * the transform feedback object and its buffers for the newly-created wing.
			 * The old data will be overwritten.
			 */
			wingTransformFeedbackObject = wings.back().getTransformFeedbackObject();
			wings.pop_back();
		}

		wings.emplace_front(
			wingTransformFeedbackObject,
			deltaAngle, deltaZ);

		/*
		 * Run a vertex shader to transform the wing based on its current
		 * animation state, and capture the transformed geometry using transform
		 * feedback.
		 */
		wingTransformProgram->TransformWing(radius, angle,
			roll, pitch, yaw,
			red, green, blue,
			*wingTransformFeedbackObject);

		glFlush();
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

		wingRenderProgram->Ortho(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	}

}
