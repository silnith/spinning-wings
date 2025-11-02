#include <Windows.h>
#include <gl/GL.h>

#include <string>
#include <sstream>

#include <cassert>

#include "GLInfo.h"

namespace silnith::wings::gl
{
    GLInfo::GLInfo(void)
    {
        GLubyte const* const glVersion{ glGetString(GL_VERSION) };

        assert(glVersion != nullptr);

		std::istringstream versionStringInput{ std::string{ reinterpret_cast<char const*>(glVersion) } };
		//std::basic_istringstream<GLubyte> versionStringInput{ std::basic_string<GLubyte>{glVersion} };

		versionStringInput >> majorVersion;
		GLubyte period;
		versionStringInput >> period;
		assert(period == '.');
		versionStringInput >> minorVersion;
    }

	bool GLInfo::isAtLeastVersion(GLuint major, GLuint minor) const
	{
		return (majorVersion > major)
			|| (majorVersion == major && minorVersion >= minor);
	}

}
