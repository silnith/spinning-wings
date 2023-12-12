#include "Program.h"

#include <cassert>
#include <cstddef>
#include <stdexcept>

namespace silnith::wings::gl2
{

    Program::~Program(void) noexcept
    {
        glDeleteProgram(id);
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif
    }

    Program::Program(Shader const& s1, Shader const& s2)
        : id{ glCreateProgram() }, linkLog{}
    {
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif
        glAttachShader(id, s1.getShader());
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif
        glAttachShader(id, s2.getShader());
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif

        glLinkProgram(id);
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif

        glDetachShader(id, s2.getShader());
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif
        glDetachShader(id, s1.getShader());
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif

        {
            GLint logSize{ 0 };
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
#ifndef NDEBUG
            {
                GLenum const error{ glGetError() };
                assert(error == GL_NO_ERROR);
            }
#endif
            if (logSize > 0) {
                GLchar* const log{ new GLchar[logSize] };
                glGetProgramInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log);
#ifndef NDEBUG
                {
                    GLenum const error{ glGetError() };
                    assert(error == GL_NO_ERROR);
                }
#endif
                linkLog = { log };
                delete[] log;
            }
        }

        GLint linkSuccess{ 0 };
        glGetProgramiv(id, GL_LINK_STATUS, &linkSuccess);
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif
        switch (linkSuccess)
        {
        case GL_TRUE:
            break;
        case GL_FALSE:
        {
            glDeleteProgram(id);
#ifndef NDEBUG
            {
                GLenum const error{ glGetError() };
                assert(error == GL_NO_ERROR);
            }
#endif
            throw std::runtime_error{ linkLog };
        }
        default:
        {
            glDeleteProgram(id);
#ifndef NDEBUG
            {
                GLenum const error{ glGetError() };
                assert(error == GL_NO_ERROR);
            }
#endif
            throw std::runtime_error{ "Unknown link status: " + linkSuccess };
        }
        }
    }

    GLuint Program::getProgram(void) const noexcept
    {
        return id;
    }

    void Program::useProgram(void) const noexcept
    {
        glUseProgram(id);
#ifndef NDEBUG
        {
            GLenum const error{ glGetError() };
            assert(error == GL_NO_ERROR);
        }
#endif
    }

}
