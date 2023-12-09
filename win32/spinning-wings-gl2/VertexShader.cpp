#include "VertexShader.h"

silnith::VertexShader::VertexShader(std::vector<std::string> const& sources)
    : Shader(GL_VERTEX_SHADER, sources)
{}
