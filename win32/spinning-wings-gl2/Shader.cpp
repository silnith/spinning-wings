#include "Shader.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std::literals::string_literals;

namespace silnith::wings::gl2
{

    Shader::Shader(GLenum type, std::initializer_list<std::string> const& sources)
        : id{ glCreateShader(type) }, compilationLog{}
    {
        assert((type == GL_VERTEX_SHADER) || (type == GL_FRAGMENT_SHADER));

        if (id == 0)
        {
            throw std::runtime_error{ "Failed to allocate GLSL shader."s };
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

        GLint logSize{ 0 };
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
        if (logSize > 0) {
            std::unique_ptr<GLchar[]> log{ std::make_unique<GLchar[]>(static_cast<std::size_t>(logSize)) };
            glGetShaderInfoLog(id, static_cast<GLsizei>(logSize), nullptr, log.get());
            compilationLog = { log.get() };
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
            std::ostringstream errorMessage{};
            errorMessage << "Unknown compilation status: "s;
            errorMessage << compilationSuccess;
            throw std::runtime_error{ errorMessage.str() };
        }
        }
    }

    Shader::~Shader(void) noexcept
    {
        glDeleteShader(id);
    }

    GLuint Shader::getShader(void) const noexcept
    {
        return id;
    }

}
