#include "FragmentShader.h"

silnith::FragmentShader::FragmentShader(std::vector<std::string> const& sources)
    : Shader(GL_FRAGMENT_SHADER, sources)
{}
