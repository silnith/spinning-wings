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

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl4
{

	typedef std::deque<Wing<GLuint, GLfloat> > wing_list;

	std::size_t constexpr numWings{ 40 };

	// TODO: Investigate glObjectLabel
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

	std::unique_ptr<Program> wingTransformProgram{ nullptr };
	std::unique_ptr<Program> renderProgram{ nullptr };

	GLsizei constexpr numVertices{ 4 };
	GLsizei constexpr numIndices{ 4 };
	/// <summary>
	/// The initial untransformed vertices for a single quad.
	/// After binding, enable using <c>glVertexAttribPointer(..., 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0)</c>.
	/// </summary>
	GLuint originalVertexBuffer{ 0 };
	/// <summary>
	/// The indices into <c>originalVertexBuffer</c>.
	/// </summary>
	GLuint wingIndexBuffer{ 0 };
	/// <summary>
	/// The vertex array used for transform feedback.
	/// This maintains the state of the enabled vertex attributes.
	/// </summary>
	GLuint wingTransformVertexArray{ 0 };
	/// <summary>
	/// The vertex array used for rendering.
	/// This maintains the state of the enabled vertex attributes,
	/// as well as the binding for the ELEMENT_ARRAY_BUFFER.
	/// </summary>
	GLuint renderVertexArray{ 0 };

	/// <summary>
	/// The uniform buffer for the ModelViewProjection matrices.
	/// </summary>
	GLuint modelViewProjectionUniformBuffer{ 0 };
	/// <summary>
	/// The uniform buffer binding index for the ModelViewProjection matrices.
	/// </summary>
	/// <remarks>
	/// <para>
	/// Uniform buffers use an indirection mechanism.  Rather than binding them directly,
	/// a shader program specifies an index into a set of binding points.
	/// The application can then update the uniform buffer bound to that index
	/// independently of whichever shader program is active.
	/// </para>
	/// <para>
	/// In this case, there is only one shader program and one uniform buffer.
	/// So this simply uses the first index in the array of binding points.
	/// </para>
	/// </remarks>
	GLuint constexpr modelViewProjectionBindingIndex{ 0 };

	/// <summary>
	/// The offset into the uniform buffer where the model matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr modelOffset{ 0 };
	/// <summary>
	/// The offset into the uniform buffer where the view matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr viewOffset{ 0 };
	/// <summary>
	/// The offset into the uniform buffer where the projection matrix is.
	/// </summary>
	/// <seealso cref="modelViewProjectionUniformBuffer"/>
	GLintptr projectionOffset{ 0 };

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
		 * Set up the initial camera position.
		 */
		glm::mat4 const view2{ glm::lookAt(
			glm::vec3{ 0, 50, 50 },
			glm::vec3{ 0, 0, 13 },
			glm::vec3{ 0, 0, 1 }) };

		/*
		 * Set up the pieces needed to render one single
		 * (untransformed, uncolored) wing.
		 */
		{
			std::array<GLfloat, 2 * numVertices> constexpr quadVertices{
				1, 1,
				-1, 1,
				-1, -1,
				1, -1,
			};
			GLsizeiptr constexpr quadVerticesDataSize{ sizeof(GLfloat) * quadVertices.size() };

			glGenBuffers(1, &originalVertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, quadVerticesDataSize, quadVertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{
			std::array<GLuint, numIndices> constexpr quadIndices{
				0, 1, 2, 3,
			};
			GLsizeiptr constexpr quadIndicesDataSize{ sizeof(GLuint) * quadIndices.size() };

			glGenBuffers(1, &wingIndexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndicesDataSize, quadIndices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		std::shared_ptr<VertexShader const> rotateMatrixShader{
			std::make_shared<VertexShader const>(std::initializer_list<std::string>{
				Shader::versionDeclaration,
				Shader::rotateMatrixFunctionDefinition,
			})
		};
		std::shared_ptr<VertexShader const> translateMatrixShader{
			std::make_shared<VertexShader const>(std::initializer_list<std::string>{
				Shader::versionDeclaration,
				Shader::translateMatrixFunctionDefinition,
			})
		};
		std::shared_ptr<VertexShader const> scaleMatrixShader{
			std::make_shared<VertexShader const>(std::initializer_list<std::string>{
				Shader::versionDeclaration,
				Shader::scaleMatrixFunctionDefinition,
			})
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
)shaderText"s,
				Shader::rotateMatrixFunctionDeclaration,
				Shader::translateMatrixFunctionDeclaration,
				R"shaderText(
const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);

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
		glBindBuffer(GL_ARRAY_BUFFER, originalVertexBuffer);
		glVertexAttribPointer(vertexAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		std::initializer_list<std::shared_ptr<VertexShader const> > renderVertexShaders{
			std::make_shared<VertexShader const>(std::initializer_list<std::string>{
				Shader::versionDeclaration,
				R"shaderText(
uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform vec2 deltaZ = vec2(15, 0.5);

in vec4 vertex;
in vec4 color;

smooth out vec4 varyingColor;
)shaderText"s,
				Shader::rotateMatrixFunctionDeclaration,
				Shader::translateMatrixFunctionDeclaration,
				R"shaderText(
const vec3 xAxis = vec3(1, 0, 0);
const vec3 yAxis = vec3(0, 1, 0);
const vec3 zAxis = vec3(0, 0, 1);

void main() {
    float deltaAngle = deltaZ[0];
    float deltaZ = deltaZ[1];

    mat4 modelViewProjection = projection * view * model;

    varyingColor = color;
    gl_Position = modelViewProjection
                  * translate(vec3(0, 0, deltaZ))
                  * rotate(deltaAngle, zAxis)
                  * vertex;
}
)shaderText"s,
			}),
			rotateMatrixShader,
			translateMatrixShader,
		};
		std::initializer_list<std::shared_ptr<FragmentShader const> > renderFragmentShaders{
			std::make_shared<FragmentShader const>(std::initializer_list<std::string>{
				Shader::versionDeclaration,
				R"shaderText(
smooth in vec4 varyingColor;

out vec4 fragmentColor;

void main() {
    fragmentColor = varyingColor;
}
)shaderText"s,
			}),
		};
		renderProgram = std::make_unique<Program>(
			renderVertexShaders,
			renderFragmentShaders,
			"fragmentColor"
		);
		glGenVertexArrays(1, &renderVertexArray);
		glBindVertexArray(renderVertexArray);
		glEnableVertexAttribArray(renderProgram->getAttributeLocation("vertex"s));
		glEnableVertexAttribArray(renderProgram->getAttributeLocation("color"s));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wingIndexBuffer);
		glBindVertexArray(0);

		glReleaseShaderCompiler();

		GLsizei dataSize{ 0 };
		{
			GLuint const programId{ renderProgram->GetName() };
			GLuint const blockIndex{ glGetUniformBlockIndex(programId, "ModelViewProjection")};
			glUniformBlockBinding(programId, blockIndex, modelViewProjectionBindingIndex);

			{
				GLsizei constexpr numUniforms{ 3 };
				std::array<GLchar const*, numUniforms> constexpr names{ "model", "view", "projection" };
				std::array<GLuint, numUniforms> uniformIndices{ 0, 0, 0 };
				glGetUniformIndices(programId, numUniforms, names.data(), uniformIndices.data());

				std::array<GLint, numUniforms> uniformOffsets{ 0, 0, 0, };
				glGetActiveUniformsiv(programId, numUniforms, uniformIndices.data(), GL_UNIFORM_OFFSET, uniformOffsets.data());
				modelOffset = uniformOffsets[0];
				viewOffset = uniformOffsets[1];
				projectionOffset = uniformOffsets[2];
			}

			{
				GLint modelViewProjectionUniformDataSize{ 0 };
				glGetActiveUniformBlockiv(programId, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &modelViewProjectionUniformDataSize);
				dataSize = modelViewProjectionUniformDataSize;
			}
		}

		glGenBuffers(1, &modelViewProjectionUniformBuffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, modelViewProjectionBindingIndex, modelViewProjectionUniformBuffer);

		/*
		 * The C++ syntax makes this look like it is row-major, but OpenGL will read it as column-major.
		 * However, that is irrelevant because the identity matrix is its own transposition.
		 */
		std::array<GLfloat, 16> constexpr identity{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};
		GLsizeiptr constexpr identityDataSize{ sizeof(GLfloat) * identity.size() };

		std::array<GLfloat, 16> const view{
			view2[0][0],
			view2[0][1],
			view2[0][2],
			view2[0][3],

			view2[1][0],
			view2[1][1],
			view2[1][2],
			view2[1][3],

			view2[2][0],
			view2[2][1],
			view2[2][2],
			view2[2][3],

			view2[3][0],
			view2[3][1],
			view2[3][2],
			view2[3][3],
		};
		GLsizeiptr constexpr viewDataSize{ sizeof(GLfloat) * view.size() };

		glBindBuffer(GL_UNIFORM_BUFFER, modelViewProjectionUniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, modelOffset, identityDataSize, identity.data());
		glBufferSubData(GL_UNIFORM_BUFFER, viewOffset, viewDataSize, view.data());
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, identityDataSize, identity.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void CleanupOpenGLState(void)
	{
		for (wing_list::const_reference wing : wings)
		{
			GLuint const vertexBuffer{ wing.getVertexBuffer() };
			GLuint const colorBuffer{ wing.getColorBuffer() };
			GLuint const edgeColorBuffer{ wing.getEdgeColorBuffer() };
			GLuint const transformFeedbackObject{ wing.getTransformFeedbackObject() };
			glDeleteTransformFeedbacks(1, &transformFeedbackObject);
			glDeleteBuffers(1, &edgeColorBuffer);
			glDeleteBuffers(1, &colorBuffer);
			glDeleteBuffers(1, &vertexBuffer);
		}

		glDeleteBuffers(1, &modelViewProjectionUniformBuffer);

		glDeleteVertexArrays(1, &renderVertexArray);
		glDeleteVertexArrays(1, &wingTransformVertexArray);

		glDeleteBuffers(1, &wingIndexBuffer);
		glDeleteBuffers(1, &originalVertexBuffer);

		wingTransformProgram.reset();
		renderProgram.reset();
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

		GLuint wingVertexBuffer{ 0 };
		GLuint wingColorBuffer{ 0 };
		GLuint wingEdgeColorBuffer{ 0 };
		GLuint wingTransformFeedbackObject{ 0 };
		if (wings.empty() || wings.size() < numWings)
		{
			glGenBuffers(1, &wingVertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, wingVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * numVertices, nullptr, GL_DYNAMIC_COPY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &wingColorBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, wingColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, nullptr, GL_DYNAMIC_COPY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &wingEdgeColorBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, wingEdgeColorBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVertices, nullptr, GL_DYNAMIC_COPY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenTransformFeedbacks(1, &wingTransformFeedbackObject);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, wingTransformFeedbackObject);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, wingVertexBuffer);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, wingColorBuffer);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, wingEdgeColorBuffer);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		}
		else
		{
			/*
			 * If a wing expires off the end of the list of wings, we can reuse
			 * the various buffers for the newly-created wing.  The old data
			 * will be overwritten.
			 */
			// This block is simply so lastWing goes out of scope before the pop_back.
			{
				wing_list::const_reference lastWing{ wings.back() };
				wingVertexBuffer = lastWing.getVertexBuffer();
				wingColorBuffer = lastWing.getColorBuffer();
				wingEdgeColorBuffer = lastWing.getEdgeColorBuffer();
				wingTransformFeedbackObject = lastWing.getTransformFeedbackObject();
			}
			wings.pop_back();
		}

		wings.emplace_front(wingVertexBuffer,
			wingColorBuffer,
			wingEdgeColorBuffer,
			wingTransformFeedbackObject,
			deltaAngle, deltaZ);

		/*
		 * Run a vertex shader to transform the wing based on its current
		 * animation state, and capture the transformed geometry using transform
		 * feedback.
		 */
		wingTransformProgram->useProgram();

		glBindVertexArray(wingTransformVertexArray);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, wingTransformFeedbackObject);

		glEnable(GL_RASTERIZER_DISCARD);
		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, numVertices);
		glEndTransformFeedback();
		glDisable(GL_RASTERIZER_DISCARD);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

		glBindVertexArray(0);
	}

	void DrawFrame(void)
	{
		renderProgram->useProgram();
		GLuint const vertexAttribLocation{ renderProgram->getAttributeLocation("vertex"s) };
		GLuint const colorAttribLocation{ renderProgram->getAttributeLocation("color"s) };

		glBindVertexArray(renderVertexArray);

		GLfloat deltaZ{ 0 };
		GLfloat deltaAngle{ 0 };
		for (wing_list::const_reference wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			glUniform2f(renderProgram->getUniformLocation("deltaZ"s), deltaAngle, deltaZ);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getVertexBuffer());
			glVertexAttribPointer(vertexAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElements(GL_TRIANGLE_FAN, numIndices, GL_UNSIGNED_INT, 0);
		}

		deltaZ = 0;
		deltaAngle = 0;
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		for (wing_list::const_reference wing : wings) {
			deltaZ += wing.getDeltaZ();
			deltaAngle += wing.getDeltaAngle();

			glUniform2f(renderProgram->getUniformLocation("deltaZ"s), deltaAngle, deltaZ);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getVertexBuffer());
			glVertexAttribPointer(vertexAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, wing.getEdgeColorBuffer());
			glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawElements(GL_LINE_LOOP, numIndices, GL_UNSIGNED_INT, 0);
		}
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		glBindVertexArray(0);

		glFlush();
	}

	/// <summary>
	/// Sets up the orthographic projection that transforms modelview coordinates
	/// into normalized device coordinates.
	/// This takes into account the aspect ratio of the viewport.
	/// </summary>
	/// <param name="width">The viewport width.</param>
	/// <param name="height">The viewport height.</param>
	void Ortho(GLfloat const width, GLfloat const height)
	{
		/*
		 * These multipliers account for the aspect ratio of the window, so that
		 * the rendering does not distort.  The conditional is so that the larger
		 * number is always divided by the smaller, resulting in a multiplier no
		 * less than one.  This way, the viewing area is always expanded rather than
		 * contracted, and the expected viewing frustum is never clipped.
		 */
		GLfloat xmult{ 1.0 };
		GLfloat ymult{ 1.0 };
		if (width > height)
		{
			xmult = width / height;
		}
		else
		{
			ymult = height / width;
		}

		/*
		 * The view frustum was hand-selected to match the parameters to the
		 * curve generators and the initial camera position.
		 */
		GLfloat constexpr defaultLeft{ -20 };
		GLfloat constexpr defaultRight{ 20 };
		GLfloat constexpr defaultBottom{ -20 };
		GLfloat constexpr defaultTop{ 20 };
		GLfloat constexpr defaultNear{ 35 };
		GLfloat constexpr defaultFar{ 105 };

		GLfloat const left{ defaultLeft * xmult };
		GLfloat const right{ defaultRight * xmult };
		GLfloat const bottom{ defaultBottom * ymult };
		GLfloat const top{ defaultTop * ymult };
		GLfloat const nearZ{ defaultNear };
		GLfloat const farZ{ defaultFar };

		GLfloat const viewWidth{ right - left };
		GLfloat const viewHeight{ top - bottom };
		GLfloat const viewDepth{ farZ - nearZ };

		assert(viewWidth > 0);
		assert(viewHeight > 0);
		assert(viewDepth > 0);

		/*
		 * Set up the projection matrix.
		 * The projection matrix is only used for the viewing frustum.
		 * Things like camera position belong in the modelview matrix.
		 */
		std::array<GLfloat, 16> const projection{
			// column 0
			static_cast<GLfloat>(2) / viewWidth,
			0,
			0,
			0,

			// column 1
			0,
			static_cast<GLfloat>(2) / viewHeight,
			0,
			0,

			// column 2
			0,
			0,
			static_cast<GLfloat>(-2) / viewDepth,
			0,

			// column 3
			-(right + left) / viewWidth,
			-(top + bottom) / viewHeight,
			-(farZ + nearZ) / viewDepth,
			static_cast<GLfloat>(1),
		};
		GLsizeiptr constexpr projectionDataSize{ sizeof(GLfloat) * projection.size() };

		glBindBuffer(GL_UNIFORM_BUFFER, modelViewProjectionUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, projectionDataSize, projection.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

		Ortho(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	}

}
