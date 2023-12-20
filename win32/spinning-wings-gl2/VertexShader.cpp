#include "VertexShader.h"

namespace silnith::wings::gl2
{

    VertexShader::VertexShader(std::initializer_list<std::string> const& sources)
        : Shader(GL_VERTEX_SHADER, sources)
    {}

}
