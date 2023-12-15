#include "Program.h"

#include <cstddef>
#include <stdexcept>

namespace silnith::wings::gl3
{

    Program::~Program(void) noexcept
    {
        glDeleteProgram(id);
    }

    Program::Program(Shader const& s1, std::vector<std::string> const& capturedVaryings)
        : id{ glCreateProgram() }, linkLog{}
    {
        glProgramParameteri(id, 0, 0);

        {
            size_t const size{ capturedVaryings.size() };
            GLchar const** varyings{ new GLchar const* [size] };
            for (size_t i{ 0 }; i < size; i++)
            {
                std::string const& varying{ capturedVaryings.at(i) };
                varyings[i] = varying.c_str();
            }
            glTransformFeedbackVaryings(id, static_cast<GLsizei>(size), varyings, GL_INTERLEAVED_ATTRIBS);
            //glTransformFeedbackVaryings(id, capturedVaryings.size(), varyings, GL_SEPARATE_ATTRIBS);
            delete[] varyings;
        }

        glAttachShader(id, s1.getShader());

        glLinkProgram(id);

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

    GLint Program::getUniformLocation(std::string const& name) const
    {
        GLint const uniformLocation{ glGetUniformLocation(id, name.c_str()) };
        if (uniformLocation < 0)
        {
            throw new std::runtime_error{ "Uniform " + name + " not bound." };
        }
        return static_cast<GLint>(uniformLocation);
    }

    void Program::useProgram(void) const noexcept
    {
        glUseProgram(id);
    }

}
