#include "Shader.h"

#include <cstddef>
#include <stdexcept>

namespace silnith::wings::gl2
{

    Shader::~Shader(void) noexcept
    {
        glDeleteShader(id);
    }

    Shader::Shader(GLenum type, std::vector<std::string> const& sources_)
        : id{ glCreateShader(type) }, compilationLog{}
    {
        {
            std::vector<std::string> sources{ sources_ };
            for (std::string& source : sources)
            {
                source += '\n';
            }

            size_t const size{ sources.size() };
            GLchar const** cSources{ new GLchar const* [size] };
            for (size_t index{ 0 }; index < size; index++)
            {
                cSources[index] = sources.at(index).c_str();
            }
            glShaderSource(id, static_cast<GLsizei>(size), cSources, nullptr);
            delete[] cSources;
        }
        glCompileShader(id);

        {
            GLint logSize{ 0 };
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
            if (logSize > 0) {
                GLchar* const log{ new GLchar[logSize] };
                glGetShaderInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log);
                compilationLog = { log };
                delete[] log;
            }
        }

        GLint compilationSuccess{ 0 };
        glGetShaderiv(id, GL_COMPILE_STATUS, &compilationSuccess);
        switch (compilationSuccess)
        {
        case GL_TRUE:
            break;
        case GL_FALSE:
        {
            glDeleteShader(id);
            throw std::runtime_error{ compilationLog };
        }
        default:
        {
            glDeleteShader(id);
            throw std::runtime_error{ "Unknown compilation status: " + compilationSuccess };
        }
        }
    }

    GLuint Shader::getShader(void) const noexcept
    {
        return id;
    }

}
