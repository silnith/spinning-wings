#include <Windows.h>
#include <GL/glew.h>

#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>

#include <cstddef>

#include "Program.h"

#include "FragmentShader.h"
#include "VertexShader.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl2
{

    Program::Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader)
        : id{ glCreateProgram() }, linkLog{}
    {
        if (id == 0)
        {
            throw std::runtime_error{ "Failed to allocate GLSL program."s };
        }

        glAttachShader(id, vertexShader.getShader());
        glAttachShader(id, fragmentShader.getShader());

        glLinkProgram(id);

        glDetachShader(id, fragmentShader.getShader());
        glDetachShader(id, vertexShader.getShader());

        GLint logSize{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
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
            std::ostringstream errorMessage{};
            errorMessage << "Unknown link status: "s;
            errorMessage << linkSuccess;
            throw std::runtime_error{ errorMessage.str() };
        }
        }
    }

    Program::~Program(void) noexcept
    {
        glDeleteProgram(id);
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
            throw std::runtime_error{ "Attribute "s + name + " not bound."s };
        }
        return static_cast<GLuint>(attributeLocation);
    }

    void Program::useProgram(void) const
    {
        glUseProgram(id);
    }

}
