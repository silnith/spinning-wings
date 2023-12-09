#include "FragmentShader.h"

silnith::gl2::FragmentShader::FragmentShader(std::vector<std::string> const& sources)
    : Shader(GL_FRAGMENT_SHADER, sources)
{}
