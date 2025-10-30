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
    class GLInfo
    {
    public:
        GLInfo(void);
        GLInfo(GLInfo const&) noexcept = default;
        GLInfo& operator=(GLInfo const&) noexcept = default;
        GLInfo(GLInfo&&) noexcept = default;
        GLInfo& operator=(GLInfo&&) noexcept = default;
        ~GLInfo(void) noexcept = default;

    public:
        /// <summary>
        /// Returns <see langword="true"/> if the OpenGL implementation is equal to or higher
        /// than the input version number.
        /// </summary>
        /// <param name="major">The major version number expected.</param>
        /// <param name="minor">The minor version number expected.</param>
        /// <returns><see langword="true"/> if the OpenGL implementation satisfies the requested version.</returns>
        bool hasOpenGL(GLuint major, GLuint minor) const;

    private:
        GLuint majorVersion{ 1 };
        GLuint minorVersion{ 0 };
    };
}
