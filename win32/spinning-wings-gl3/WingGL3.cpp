#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "WingGL3.h"

#include "ArrayBuffer.h"

namespace silnith::wings::gl3
{

	Wing::Wing(GLfloat deltaAngle, GLfloat deltaZ,
		std::shared_ptr<ArrayBuffer> const& vertexBuffer,
		std::shared_ptr<ArrayBuffer> const& colorBuffer,
		std::shared_ptr<ArrayBuffer> const& edgeColorBuffer) noexcept
		: deltaAngle{ deltaAngle }, deltaZ{ deltaZ },
		vertexBuffer{ vertexBuffer }, colorBuffer{ colorBuffer }, edgeColorBuffer{ edgeColorBuffer }
	{}

	GLfloat Wing::getDeltaAngle(void) const noexcept
	{
		return deltaAngle;
	}

	GLfloat Wing::getDeltaZ(void) const noexcept
	{
		return deltaZ;
	}

	std::shared_ptr<ArrayBuffer> Wing::getVertexBuffer(void) const noexcept
	{
		return vertexBuffer;
	}

	std::shared_ptr<ArrayBuffer> Wing::getColorBuffer(void) const noexcept
	{
		return colorBuffer;
	}

	std::shared_ptr<ArrayBuffer> Wing::getEdgeColorBuffer(void) const noexcept
	{
		return edgeColorBuffer;
	}

	GLuint Wing::getVertexBufferId(void) const noexcept
	{
		return vertexBuffer->getId();
	}

	GLuint Wing::getColorBufferId(void) const noexcept
	{
		return colorBuffer->getId();
	}

	GLuint Wing::getEdgeColorBufferId(void) const noexcept
	{
		return edgeColorBuffer->getId();
	}

}
