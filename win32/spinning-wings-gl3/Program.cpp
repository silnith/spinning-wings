#include "Program.h"

#include <cstddef>
#include <stdexcept>

namespace silnith::wings::gl3
{

    Program::~Program(void) noexcept
    {
        glDeleteProgram(id);
    }

    Program::Program(Shader const& s1, Shader const& s2)
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

    GLuint Program::getProgram(void) const noexcept
    {
        return id;
    }

    GLuint Program::getAttributeLocation(std::string const& name) const
    {
        GLint const attributeLocation{ glGetAttribLocation(id, name.c_str()) };
        if (attributeLocation < 0)
        {
            throw new std::runtime_error{ "Attribute " + name + " not bound." };
        }
        return static_cast<GLuint>(attributeLocation);
    }

    void Program::useProgram(void) const noexcept
    {
        glUseProgram(id);
    }

}
