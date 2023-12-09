#include "Program.h"

#include <cstddef>
#include <stdexcept>

silnith::Program::Program(Program&& program) noexcept
    : id{ program.id }, linkLog{ program.linkLog }
{
    program.id = 0;
    program.linkLog = {};
}

silnith::Program& silnith::Program::operator=(Program&& program) noexcept
{
    id = program.id;
    linkLog = program.linkLog;
    program.id = 0;
    program.linkLog = {};
    return *this;
}

silnith::Program::Program(silnith::Shader const& s1, silnith::Shader const& s2)
    : id{ glCreateProgram() }, linkLog{}
{
    glAttachShader(id, s1.getShader());
    glAttachShader(id, s2.getShader());

    glLinkProgram(id);

    glDetachShader(id, s2.getShader());
    glDetachShader(id, s1.getShader());

    {
        GLint logSize{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            GLchar* const log{ new GLchar[logSize] };
            glGetProgramInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log);
            linkLog = { log };
            delete[] log;
        }
    }

    GLint linkSuccess{ 0 };
    glGetProgramiv(id, GL_LINK_STATUS, &linkSuccess);
    switch (linkSuccess)
    {
    case GL_TRUE:
        break;
    case GL_FALSE:
    {
        glDeleteProgram(id);
        throw std::runtime_error{ linkLog };
    }
    default:
    {
        glDeleteProgram(id);
        throw std::runtime_error{ "Unknown link status: " + linkSuccess };
    }
    }
}

silnith::Program::~Program(void) noexcept
{
    glDeleteProgram(id);
}

GLuint silnith::Program::getProgram(void) const noexcept
{
    return id;
}

void silnith::Program::useProgram(void) const noexcept
{
    glUseProgram(id);
}
