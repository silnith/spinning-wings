#include <Windows.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

#include <cassert>

#include "ModelViewProjectionUniformBuffer.h"

namespace silnith::wings::gl3
{

	ModelViewProjectionUniformBuffer::ModelViewProjectionUniformBuffer(GLsizei dataSize,
		GLintptr modelOffset,
		GLintptr viewOffset,
		GLintptr projectionOffset)
		: Buffer{},
		modelOffset{ modelOffset },
		viewOffset{ viewOffset },
		projectionOffset{ projectionOffset }
	{
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

		/*
		 * Set up the initial camera position.
		 */
		glm::mat4 const view2{ glm::lookAt(
			glm::vec3{ 0, 50, 50 },
			glm::vec3{ 0, 0, 13 },
			glm::vec3{ 0, 0, 1 }) };

		std::array<GLfloat, 4 * 4> const view{
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

		glBindBuffer(GL_UNIFORM_BUFFER, getId());
		/*
		 * First we need to allocate storage space for the uniform buffer.
		 */
		glBufferData(GL_UNIFORM_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
		/*
		 * Then we initialize the components of the uniform block.
		 * Each component is initialized separately.
		 */
		glBufferSubData(GL_UNIFORM_BUFFER, modelOffset, identityDataSize, identity.data());
		glBufferSubData(GL_UNIFORM_BUFFER, viewOffset, viewDataSize, view.data());
		//glBufferSubData(GL_UNIFORM_BUFFER, viewOffset, viewDataSize, glm::value_ptr(view2));
		/*
		 * The projection matrix is initialized here to the identity matrix.
		 * It will be replaced with the real projection matrix later when the window is resized.
		 */
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, identityDataSize, identity.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void ModelViewProjectionUniformBuffer::SetProjectionMatrix(GLfloat width, GLfloat height)
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
		std::array<GLfloat, 4 * 4> const projection{
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
		//glm::mat4 const foo{ glm::ortho(defaultLeft * xmult, defaultRight * xmult,
		//	defaultBottom * ymult, defaultTop * ymult,
		//	defaultNear, defaultFar) };
		//GLfloat const* bar{ glm::value_ptr(foo) };
		GLsizeiptr constexpr projectionDataSize{ sizeof(GLfloat) * projection.size() };

		glBindBuffer(GL_UNIFORM_BUFFER, getId());
		glBufferSubData(GL_UNIFORM_BUFFER, projectionOffset, projectionDataSize, projection.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}
