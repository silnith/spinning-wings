#include "Shader.h"

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace silnith::wings::gl3
{

    Shader::~Shader(void) noexcept
    {
        glDeleteShader(id);
    }

    Shader::Shader(GLenum type, std::initializer_list<std::string> const& sources)
        : id{ glCreateShader(type) }, compilationLog{}
    {
        if (id == 0)
        {
            throw new std::runtime_error{ "Failed to allocate shader." };
        }

        // Limit scope of copies of string sources.
        {
            std::vector<std::string> sourcesCopy{};
            sourcesCopy.reserve(sources.size());
            for (std::string const& source : sources)
            {
                sourcesCopy.emplace_back(source + '\n');
            }

            std::vector<GLchar const*> cSources{};
            cSources.reserve(sourcesCopy.size());
            for (std::string const& source : sourcesCopy)
            {
                cSources.emplace_back(source.c_str());
            }
            glShaderSource(id, static_cast<GLsizei>(cSources.size()), cSources.data(), nullptr);
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
