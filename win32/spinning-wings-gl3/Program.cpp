#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <cstddef>

#include "Program.h"

#include "FragmentShader.h"
#include "VertexShader.h"

using namespace std::literals::string_literals;

namespace silnith::wings::gl3
{

    Program::Program(VertexShader const& vertexShader, std::initializer_list<std::string> const& capturedVaryings)
        : id{ glCreateProgram() },
        linkLog{}
    {
        // Limit scope of pointers to string sources.
        {
            std::vector<GLchar const*> cPtrs{};
            for (std::string const& capturedVarying : capturedVaryings)
            {
                cPtrs.emplace_back(capturedVarying.c_str());
            }
            GLsizei const count{ static_cast<GLsizei>(cPtrs.size()) };
            GLchar const** varyings{ cPtrs.data() };
            glTransformFeedbackVaryings(id, count, varyings, GL_SEPARATE_ATTRIBS);
        }

        glAttachShader(id, vertexShader.getShader());

        glLinkProgram(id);

        glDetachShader(id, vertexShader.getShader());

        GLint logSize{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetProgramInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log.get());
            linkLog = std::string{ log.get() };
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

    Program::Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader, std::string const& fragmentData)
        : id{ glCreateProgram() }, linkLog{}
    {
        /*
         * The color number allow rendering to multiple draw buffers.
         */
        GLuint constexpr colorNumber{ 0 };
        glBindFragDataLocation(id, colorNumber, fragmentData.c_str());

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
            linkLog = std::string{ log.get() };
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

    void Program::Validate(void) const
    {
        glValidateProgram(id);
        GLint validationStatus{ 0 };
        glGetProgramiv(id, GL_VALIDATE_STATUS, &validationStatus);

        std::string validationLog{};

        GLint logSize{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetProgramInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log.get());
            validationLog = std::string{ log.get() };
        }

        switch (validationStatus)
        {
        case GL_TRUE: break;
        case GL_FALSE:
        {
            throw std::runtime_error{ validationLog };
        }
        default:
        {
            std::ostringstream errorMessage{};
            errorMessage << "Unknown validation status: "s;
            errorMessage << validationStatus;
            throw std::runtime_error{ errorMessage.str() };
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
            throw std::runtime_error{ "Attribute "s + name + " not bound."s };
        }
        return static_cast<GLuint>(attributeLocation);
    }

    GLint Program::getUniformLocation(std::string const& name) const
    {
        GLint const uniformLocation{ glGetUniformLocation(id, name.c_str()) };
        if (uniformLocation < 0)
        {
            throw std::runtime_error{ "Uniform "s + name + " not bound."s };
        }
        return static_cast<GLint>(uniformLocation);
    }

    void Program::useProgram(void) const
    {
        glUseProgram(id);
    }

}
