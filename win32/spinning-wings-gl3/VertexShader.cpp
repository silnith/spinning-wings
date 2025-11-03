#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

#include "VertexShader.h"

namespace silnith::wings::gl3
{

    VertexShader::VertexShader(std::initializer_list<std::string> const& sources)
        : Shader{ GL_VERTEX_SHADER, sources }
    {}

}
