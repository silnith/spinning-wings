#include "WingGL3.h"

namespace silnith::wings::gl3
{

	Wing::Wing(unsigned int vertexBuffer,
		unsigned int colorBuffer,
		unsigned int edgeColorBuffer,
		float deltaAngle, float deltaZ) noexcept
		: vertexBuffer{ vertexBuffer }, colorBuffer{ colorBuffer }, edgeColorBuffer{ edgeColorBuffer },
		deltaAngle{ deltaAngle }, deltaZ{ deltaZ }
	{}

	unsigned int Wing::getVertexBuffer(void) const noexcept
	{
		return vertexBuffer;
	}

	unsigned int Wing::getColorBuffer(void) const noexcept
	{
		return colorBuffer;
	}

	unsigned int Wing::getEdgeColorBuffer(void) const noexcept
	{
		return edgeColorBuffer;
	}

	float Wing::getDeltaAngle(void) const noexcept
	{
		return deltaAngle;
	}

	float Wing::getDeltaZ(void) const noexcept
	{
		return deltaZ;
	}

}
