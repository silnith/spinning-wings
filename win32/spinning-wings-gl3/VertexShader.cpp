#include "VertexShader.h"

namespace silnith::wings::gl3
{

    VertexShader::VertexShader(std::vector<std::string> const& sources)
        : Shader(GL_VERTEX_SHADER, sources)
    {}

    VertexShader::VertexShader(std::initializer_list<std::string> const& sources)
        : Shader(GL_VERTEX_SHADER, sources)
    {}

}
