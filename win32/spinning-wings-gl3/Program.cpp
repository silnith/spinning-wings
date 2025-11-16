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
        : name{ glCreateProgram() },
        linkLog{}
    {
        if (name == 0)
        {
            throw std::runtime_error{ "Failed to create GLSL program object."s };
        }

        {
            /*
             * Since this is largely demonstration code, I have gone out of my
             * way to make the variable types explicit and precise.
             */
            std::vector<GLchar const*> cPtrs{};
            for (std::string const& capturedVarying : capturedVaryings)
            {
                cPtrs.emplace_back(capturedVarying.c_str());
            }
            GLsizei const count{ static_cast<GLsizei>(cPtrs.size()) };
            GLchar const** varyings{ cPtrs.data() };
            glTransformFeedbackVaryings(name, count, varyings, GL_SEPARATE_ATTRIBS);
        }

        /*
         * In order to create a GLSL program, compiled shaders must be attached
         * to it and then the program linked.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glAttachShader(name, vertexShader->GetName());
        }

        glLinkProgram(name);

        /*
         * Once the program is linked, the shaders are no longer needed and may
         * be detached.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glDetachShader(name, vertexShader->GetName());
        }

        /*
         * Whether the link succeeded or failed, a log may be created.  The
         * length of the log must be queried in order to know how much buffer
         * space is needed to retrieve the log.  The length will include the
         * terminating null character.  If the length is zero, there is nothing
         * in the log.
         */
        GLint logSize{ 0 };
        glGetProgramiv(name, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetProgramInfoLog(name, static_cast<GLsizei>(logSize), nullptr, log.get());
            linkLog = std::string{ log.get() };
        }

        /*
         * The GLSL program may not have linked correctly.  If linking failed,
         * the program is not valid and cannot be used.
         */
        GLint linkSuccess{ 0 };
        glGetProgramiv(name, GL_LINK_STATUS, &linkSuccess);
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
            glDeleteProgram(name);
            throw std::runtime_error{ linkLog };
        }
        default:
        {
            /*
             * The link status will either be GL_TRUE or GL_FALSE, so this
             * case will never execute on a conforming OpenGL implementation.
             * But the C++ compiler has no way to prove that.
             */
            glDeleteProgram(name);
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
        : name{ glCreateProgram() }, linkLog{}
    {
        if (name == 0)
        {
            throw std::runtime_error{ "Failed to create GLSL program object."s };
        }

        /*
         * The color number allow rendering to multiple draw buffers.
         * The default framebuffer is 0.
         * If rendering to multiple framebuffers, the color numbers must match
         * the indices of the parameters to glDrawBuffers.
         */
        GLuint constexpr colorNumber{ 0 };
        glBindFragDataLocation(name, colorNumber, fragmentData.c_str());

        /*
         * In order to create a GLSL program, compiled shaders must be attached
         * to it and then the program linked.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glAttachShader(name, vertexShader->GetName());
        }
        for (std::shared_ptr<FragmentShader const> const& fragmentShader : fragmentShaders)
        {
            glAttachShader(name, fragmentShader->GetName());
        }

        glLinkProgram(name);

        /*
         * Once the program is linked, the shaders are no longer needed and may
         * be detached.
         */
        for (std::shared_ptr<VertexShader const> const& vertexShader : vertexShaders)
        {
            glDetachShader(name, vertexShader->GetName());
        }
        for (std::shared_ptr<FragmentShader const> const& fragmentShader : fragmentShaders)
        {
            glDetachShader(name, fragmentShader->GetName());
        }

        /*
         * Whether the link succeeded or failed, a log may be created.  The
         * length of the log must be queried in order to know how much buffer
         * space is needed to retrieve the log.  The length will include the
         * terminating null character.  If the length is zero, there is nothing
         * in the log.
         */
        GLint logSize{ 0 };
        glGetProgramiv(name, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetProgramInfoLog(name, static_cast<GLsizei>(logSize), nullptr, log.get());
            linkLog = std::string{ log.get() };
        }

        /*
         * The GLSL program may not have linked correctly.  If linking failed,
         * the program is not valid and cannot be used.
         */
        GLint linkSuccess{ 0 };
        glGetProgramiv(name, GL_LINK_STATUS, &linkSuccess);
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
            glDeleteProgram(name);
            throw std::runtime_error{ linkLog };
        }
        default:
        {
            /*
             * The link status will either be GL_TRUE or GL_FALSE, so this
             * case will never execute on a conforming OpenGL implementation.
             * But the C++ compiler has no way to prove that.
             */
            glDeleteProgram(name);
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
        glDeleteProgram(name);
    }

    void Program::Validate(void) const
    {
        glValidateProgram(name);
        GLint validationStatus{ 0 };
        glGetProgramiv(name, GL_VALIDATE_STATUS, &validationStatus);

        /*
         * The validation will replace the program info log with a new one.
         */
        std::string validationLog{};

        GLint logSize{ 0 };
        glGetProgramiv(name, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetProgramInfoLog(name, static_cast<GLsizei>(logSize), nullptr, log.get());
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

    GLuint Program::GetName(void) const noexcept
    {
        return name;
    }

    GLuint Program::getAttributeLocation(std::string const& attributeName) const
    {
        /*
         * Attribute locations are specified as unsigned integers.
         * But the query returns a signed integer so that a negative return
         * value can be used to indicate an error.
         */
        GLint const attributeLocation{ glGetAttribLocation(name, attributeName.c_str()) };
        if (attributeLocation < 0)
        {
            throw std::runtime_error{ "Attribute "s + attributeName + " not bound."s };
        }
        return static_cast<GLuint>(attributeLocation);
    }

    GLint Program::getUniformLocation(std::string const& uniformName) const
    {
        /*
         * Uniform locations are of type GLint, so no casting is necessary.
         * Attempts to set a uniform variable using a location of -1 will be
         * silently ignored.
         */
        GLint const uniformLocation{ glGetUniformLocation(name, uniformName.c_str()) };
        if (uniformLocation < 0)
        {
            throw std::runtime_error{ "Uniform "s + uniformName + " not bound."s };
        }
        return static_cast<GLint>(uniformLocation);
    }

    void Program::useProgram(void) const
    {
        glUseProgram(name);
    }

}
