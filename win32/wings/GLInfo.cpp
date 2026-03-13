#include <Windows.h>
#include <gl/GL.h>

#include <string>
#include <sstream>

#include <cassert>

#include "GLInfo.h"

namespace silnith::wings::gl
{

	GLInfo::GLInfo(void) : GLInfo{ glGetString(GL_VERSION) }
    {
		//std::basic_istringstream<GLubyte> versionStringInput{ std::basic_string<GLubyte>{glVersion} };
    }

	GLInfo::GLInfo(GLubyte const* const versionString) : GLInfo{ reinterpret_cast<char const* const>(versionString) }
	{
		assert(versionString != nullptr);
	}

	GLInfo::GLInfo(char const* const versionString) : GLInfo{ std::string{ versionString } }
	{
		assert(versionString != nullptr);
	}

	GLInfo::GLInfo(std::string const& versionString)
	{
		std::istringstream versionStringInput{ versionString };

		versionStringInput >> majorVersion;
		GLubyte period;
		versionStringInput >> period;
		assert(period == '.');
		versionStringInput >> minorVersion;
	}

	GLuint GLInfo::getMajorVersion(void) const
	{
		return majorVersion;
	}

	GLuint GLInfo::getMinorVersion(void) const
	{
		return minorVersion;
	}

	bool GLInfo::isAtLeastVersion(GLuint major, GLuint minor) const
	{
		return (majorVersion > major)
			|| (majorVersion == major && minorVersion >= minor);
	}

}
