#include "Program.h"

#include <cstddef>
#include <memory>
#include <stdexcept>

namespace silnith::wings::gl2
{

    Program::~Program(void) noexcept
    {
        glDeleteProgram(id);
    }

    Program::Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader)
        : id{ glCreateProgram() }, linkLog{}
    {
        glAttachShader(id, vertexShader.getShader());
        glAttachShader(id, fragmentShader.getShader());

        glLinkProgram(id);

        glDetachShader(id, fragmentShader.getShader());
        glDetachShader(id, vertexShader.getShader());

        GLint logSize{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(logSize) };
            glGetProgramInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log.get());
            linkLog = { log.get() };
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
