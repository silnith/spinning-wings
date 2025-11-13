#include <Windows.h>
#include <GL/glew.h>

#include "QuadRendererGL10.h"

namespace silnith::wings::gl
{

	QuadRendererGL10::QuadRendererGL10(void) : WingRenderer{},
		quadDisplayList{ glGenLists(1) }
	{
		glNewList(quadDisplayList, GL_COMPILE);
		glBegin(GL_QUADS);
		glVertex2f(1, 1);
		glVertex2f(-1, 1);
		glVertex2f(-1, -1);
		glVertex2f(1, -1);
		glEnd();
		glEndList();
	}

	QuadRendererGL10::~QuadRendererGL10(void) noexcept
	{
		glDeleteLists(quadDisplayList, 1);
	}

    void QuadRendererGL10::DrawQuad(void) const
	{
		glCallList(quadDisplayList);
	}

}
