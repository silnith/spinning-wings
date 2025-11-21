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

#include "WingRenderProgram.h"

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

	std::unique_ptr<Program> wingTransformProgram{ nullptr };
	std::unique_ptr<WingRenderProgram> wingRenderProgram{ nullptr };

	std::shared_ptr<WingGeometry const> wingGeometry{ nullptr };
	/// <summary>
	/// The vertex array used for transform feedback.
	/// This maintains the state of the enabled vertex attributes.
	/// </summary>
	GLuint wingTransformVertexArray{ 0 };

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
		wingGeometry = std::make_shared<WingGeometry const>();

		std::shared_ptr<VertexShader const> rotateMatrixShader{
			VertexShader::MakeRotateMatrixShader()
		};
		std::shared_ptr<VertexShader const> translateMatrixShader{
			VertexShader::MakeTranslateMatrixShader()
		};
		std::shared_ptr<VertexShader const> scaleMatrixShader{
			VertexShader::MakeScaleMatrixShader()
		};
		std::initializer_list<std::shared_ptr<VertexShader const> > transformVertexShaders{
			std::make_shared<VertexShader const>(std::initializer_list<std::string>{
				Shader::versionDeclaration,
				R"shaderText(
uniform vec2 radiusAngle;
uniform vec3 rollPitchYaw;
uniform vec3 color;
uniform vec3 edgeColor = vec3(1, 1, 1);

in vec4 vertex;

smooth out vec3 varyingWingColor;
smooth out vec3 varyingEdgeColor;

const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);
)shaderText"s,
				Shader::rotateMatrixFunctionDeclaration,
				Shader::translateMatrixFunctionDeclaration,
				R"shaderText(
void main() {
    float radius = radiusAngle[0];
    float angle = radiusAngle[1];
    float roll = rollPitchYaw[0];
    float pitch = rollPitchYaw[1];
    float yaw = rollPitchYaw[2];

    varyingWingColor = color;
    varyingEdgeColor = edgeColor;

    mat4 wingTransformation = rotate(angle, zAxis)
                              * translate(vec3(radius, 0, 0))
                              * rotate(-yaw, zAxis)
                              * rotate(-pitch, yAxis)
                              * rotate(roll, xAxis);
    gl_Position = wingTransformation * vertex;
}
)shaderText"s,
			}),
			rotateMatrixShader,
			translateMatrixShader,
		};
		wingTransformProgram = std::make_unique<Program>(
			transformVertexShaders,
			std::initializer_list<std::string>{
				"gl_Position"s,
				"varyingWingColor"s,
				"varyingEdgeColor"s,
			}
		);
		GLuint const vertexAttributeLocation{ wingTransformProgram->getAttributeLocation("vertex") };
		glGenVertexArrays(1, &wingTransformVertexArray);
		glBindVertexArray(wingTransformVertexArray);
		glEnableVertexAttribArray(vertexAttributeLocation);
		wingGeometry->UseForVertexAttribute(vertexAttributeLocation);
		glBindVertexArray(0);

		wingRenderProgram = std::make_unique<WingRenderProgram>(wingGeometry, rotateMatrixShader, translateMatrixShader);

		glReleaseShaderCompiler();
	}

	void CleanupOpenGLState(void)
	{
		wings.clear();

		glDeleteVertexArrays(1, &wingTransformVertexArray);

		wingGeometry = nullptr;

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

		glProgramUniform2f(wingTransformProgram->GetName(), wingTransformProgram->getUniformLocation("radiusAngle"s), radius, angle);
		glProgramUniform3f(wingTransformProgram->GetName(), wingTransformProgram->getUniformLocation("rollPitchYaw"s), roll, pitch, yaw);
		glProgramUniform3f(wingTransformProgram->GetName(), wingTransformProgram->getUniformLocation("color"s), red, green, blue);
		//glProgramUniform3f(wingTransformProgram->GetName(), wingTransformProgram->getUniformLocation("edgeColor"s), 1, 1, 1);

		std::shared_ptr<WingTransformFeedback const> wingTransformFeedbackObject{ nullptr };
		if (wings.empty() || wings.size() < numWings)
		{
			std::shared_ptr<ArrayBuffer const> const wingVertexBuffer{ wingGeometry->CreateBuffer(4) };
			std::shared_ptr<ArrayBuffer const> const wingColorBuffer{ wingGeometry->CreateBuffer(3) };
			std::shared_ptr<ArrayBuffer const> const wingEdgeColorBuffer{ wingGeometry->CreateBuffer(3) };

			wingTransformFeedbackObject = std::make_shared<WingTransformFeedback const>(
				wingVertexBuffer,
				wingColorBuffer,
				wingEdgeColorBuffer);
		}
		else
		{
			/*
			 * If a wing expires off the end of the list of wings, we can reuse
			 * the various buffers for the newly-created wing.  The old data
			 * will be overwritten.
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
		wingTransformProgram->useProgram();

		glBindVertexArray(wingTransformVertexArray);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, wingTransformFeedbackObject->GetName());

		glEnable(GL_RASTERIZER_DISCARD);
		glBeginTransformFeedback(GL_POINTS);
		wingGeometry->RenderAsPoints();
		glEndTransformFeedback();
		glDisable(GL_RASTERIZER_DISCARD);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

		glBindVertexArray(0);
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
