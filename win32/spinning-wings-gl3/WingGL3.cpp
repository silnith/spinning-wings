#include <Windows.h>
#include <GL/glew.h>

#include <memory>

#include "WingGL3.h"

#include "ArrayBuffer.h"

namespace silnith::wings::gl3
{

	Wing::Wing(GLfloat deltaAngle, GLfloat deltaZ,
		std::shared_ptr<ArrayBuffer const> const& vertexBuffer,
		std::shared_ptr<ArrayBuffer const> const& colorBuffer,
		std::shared_ptr<ArrayBuffer const> const& edgeColorBuffer) noexcept
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

	std::shared_ptr<ArrayBuffer const> Wing::getVertexBuffer(void) const noexcept
	{
		return vertexBuffer;
	}

	std::shared_ptr<ArrayBuffer const> Wing::getColorBuffer(void) const noexcept
	{
		return colorBuffer;
	}

	std::shared_ptr<ArrayBuffer const> Wing::getEdgeColorBuffer(void) const noexcept
	{
		return edgeColorBuffer;
	}

}
