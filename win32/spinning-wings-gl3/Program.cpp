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

    Program::Program(std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
        std::initializer_list<std::string> capturedVaryings)
        : id{ glCreateProgram() },
        linkLog{}
    {
        if (id == 0)
        {
            throw std::runtime_error{ "Failed to allocate GLSL program."s };
        }

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

        /*
         * In order to create a GLSL program, compiled shaders must be attached
         * to it and then the program linked.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glAttachShader(id, vertexShader->getShader());
        }

        glLinkProgram(id);

        /*
         * Once the program is linked, the shaders are no longer needed and may
         * be detached.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glDetachShader(id, vertexShader->getShader());
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
            linkLog = std::string{ log.get() };
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

    Program::Program(
        std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
        std::initializer_list<std::shared_ptr<FragmentShader const> > fragmentShaders,
        std::string const& fragmentData)
        : id{ glCreateProgram() }, linkLog{}
    {
        if (id == 0)
        {
            throw std::runtime_error{ "Failed to allocate GLSL program."s };
        }

        /*
         * The color number allow rendering to multiple draw buffers.
         */
        GLuint constexpr colorNumber{ 0 };
        glBindFragDataLocation(id, colorNumber, fragmentData.c_str());

        /*
         * In order to create a GLSL program, compiled shaders must be attached
         * to it and then the program linked.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glAttachShader(id, vertexShader->getShader());
        }
        for (std::shared_ptr<FragmentShader const> const& fragmentShader : fragmentShaders)
        {
            glAttachShader(id, fragmentShader->getShader());
        }

        glLinkProgram(id);

        /*
         * Once the program is linked, the shaders are no longer needed and may
         * be detached.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glDetachShader(id, vertexShader->getShader());
        }
        for (std::shared_ptr<FragmentShader const> const& fragmentShader : fragmentShaders)
        {
            glDetachShader(id, fragmentShader->getShader());
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
            linkLog = std::string{ log.get() };
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

    void Program::Validate(void) const
    {
        glValidateProgram(id);
        GLint validationStatus{ 0 };
        glGetProgramiv(id, GL_VALIDATE_STATUS, &validationStatus);

        /*
         * The validation will replace the program info log with a new one.
         */
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
            /*
             * The validation status will either be GL_TRUE or GL_FALSE, so this
             * case will never execute on a conforming OpenGL implementation.
             * But the C++ compiler has no way to prove that.
             */
            std::ostringstream errorMessage{ "Unknown validation status: "s };
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

    GLint Program::getUniformLocation(std::string const& name) const
    {
        /*
         * Uniform locations are of type GLint, so no casting is necessary.
         * Attempts to set a uniform variable using a location of -1 will be
         * silently ignored.
         */
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
