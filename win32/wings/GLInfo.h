#pragma once

#include <Windows.h>
#include <gl/GL.h>

namespace silnith::wings::gl
{
    /// <summary>
    /// A simple class to query and expose basic information about the OpenGL
    /// implementation.  This class makes OpenGL queries, so the GL context
    /// must already exist.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Starting with OpenGL 3.0, this can be replaced with calls that query
    /// the version number directly.
    /// <code>
    /// GLint glMajorVersion{ 1 };
    /// GLint glMinorVersion{ 0 };
    /// glGetIntegerv(GL_MAJOR_VERSION, &amp;glMajorVersion);
    /// glGetIntegerv(GL_MINOR_VERSION, &amp;glMinorVersion);
    /// 
    /// assert(glMajorVersion == 3);
    /// assert(glMinorVersion == 0);
    /// </code>
    /// </para>
    /// </remarks>
    class GLInfo
    {
    public:
        GLInfo(void);

#pragma region Rule of Five

    public:
        GLInfo(GLInfo const&) = default;
        GLInfo& operator=(GLInfo const&) = default;
        GLInfo(GLInfo&&) noexcept = default;
        GLInfo& operator=(GLInfo&&) noexcept = default;
        ~GLInfo(void) noexcept = default;

#pragma endregion

    public:
        /// <summary>
        /// Returns <see langword="true"/> if the OpenGL implementation is equal to or higher
        /// than the input version number.
        /// </summary>
        /// <param name="major">The major version number expected.</param>
        /// <param name="minor">The minor version number expected.</param>
        /// <returns><see langword="true"/> if the OpenGL implementation satisfies the requested version.</returns>
        bool isAtLeastVersion(GLuint major, GLuint minor) const;

    private:
        GLuint majorVersion{ 1 };
        GLuint minorVersion{ 0 };
    };
}
