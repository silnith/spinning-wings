#include "FragmentShader.h"

namespace silnith::wings::gl4
{

    FragmentShader::FragmentShader(std::initializer_list<std::string> const& sources)
        : Shader(GL_FRAGMENT_SHADER, sources)
    {}

}
