#include "Program.h"

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace silnith::wings::gl4
{

    Program::~Program(void) noexcept
    {
        glDeleteProgram(id);
    }

    Program::Program(VertexShader const& vertexShader, std::initializer_list<std::string> const& capturedVaryings)
        : id{ glCreateProgram() }, linkLog{}
    {
        // Limit scope of pointers to string sources.
        {
            std::vector<GLchar const*> cPtrs{};
            for (std::string const& capturedVarying : capturedVaryings)
            {
                cPtrs.emplace_back(capturedVarying.c_str());
            }
            glTransformFeedbackVaryings(id, static_cast<GLsizei>(cPtrs.size()), cPtrs.data(), GL_SEPARATE_ATTRIBS);
        }

        glAttachShader(id, vertexShader.getShader());

        glLinkProgram(id);

        glDetachShader(id, vertexShader.getShader());

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

    Program::Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader, std::string const& fragmentData)
        : id{ glCreateProgram() }, linkLog{}
    {
        glBindFragDataLocation(id, 0, fragmentData.c_str());

        glAttachShader(id, vertexShader.getShader());
        glAttachShader(id, fragmentShader.getShader());

        glLinkProgram(id);

        glDetachShader(id, fragmentShader.getShader());
        glDetachShader(id, vertexShader.getShader());

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
