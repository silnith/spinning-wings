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

    Program::Program(std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
        std::initializer_list<std::shared_ptr<FragmentShader const> > fragmentShaders)
        : id{ glCreateProgram() }, linkLog{}
    {
        if (id == 0)
        {
            throw std::runtime_error{ "Failed to allocate GLSL program."s };
        }

        /*
         * In order to create a GLSL program, compiled shaders must be attached
         * to it and then the program linked.
         */
        for (std::shared_ptr<VertexShader const> const& shader : vertexShaders)
        {
            glAttachShader(id, shader->getShader());
        }
        for (std::shared_ptr<FragmentShader const> const& shader : fragmentShaders)
        {
            glAttachShader(id, shader->getShader());
        }

        glLinkProgram(id);

        /*
         * Once the program is linked, the shaders are no longer needed and may
         * be detached.
         */
        for (std::shared_ptr<VertexShader const> const& shader : vertexShaders)
        {
            glDetachShader(id, shader->getShader());
        }
        for (std::shared_ptr<FragmentShader const> const& shader : fragmentShaders)
        {
            glDetachShader(id, shader->getShader());
        }

        /*
         * Whether the link succeeded or failed, a log may be created.  The
         * length of the log must be queried in order to know how much buffer
         * space is needed to retrieve the log.  The length will include the
         * terminating null character.  If the length is zero, there is nothing
         * in the log.
         */
        GLint logSize{ 0 };
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetProgramInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log.get());
            linkLog = { log.get() };
        }

        /*
         * The GLSL program may not have linked correctly.  If linking failed,
         * the program is not valid and cannot be used.
         */
        GLint linkSuccess{ 0 };
        glGetProgramiv(id, GL_LINK_STATUS, &linkSuccess);
        switch (linkSuccess)
        {
        case GL_TRUE:
            break;
        case GL_FALSE:
        {
            /*
             * Since throwing an exception cancels construction of the C++
             * object, the destructor will never be called.  Therefore we need
             * to clean up the allocated program before throwing.
             */
            glDeleteProgram(id);
            throw std::runtime_error{ linkLog };
        }
        default:
        {
            /*
             * The link status will either be GL_TRUE or GL_FALSE, so this
             * case will never execute on a conforming OpenGL implementation.
             * But the C++ compiler has no way to prove that.
             */
            glDeleteProgram(id);
            std::ostringstream errorMessage{ "Unknown link status: "s };
            errorMessage << linkSuccess;
            throw std::runtime_error{ errorMessage.str() };
        }
        }
    }

    Program::~Program(void) noexcept
    {
        /*
         * Passing zero to the delete function will be silently ignored.
         */
        glDeleteProgram(id);
    }

    GLuint Program::getProgram(void) const noexcept
    {
        return id;
    }

    GLuint Program::getAttributeLocation(std::string const& name) const
    {
        /*
         * Attribute locations are specified as unsigned integers.
         * But the query returns a signed integer so that a negative return
         * value can be used to indicate an error.
         */
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
