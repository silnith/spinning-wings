#include "VertexShader.h"

silnith::gl2::VertexShader::VertexShader(std::vector<std::string> const& sources)
    : Shader(GL_VERTEX_SHADER, sources)
{}
