#include "FragmentShader.h"

namespace silnith::wings::gl3
{

    FragmentShader::FragmentShader(std::vector<std::string> const& sources)
        : Shader(GL_FRAGMENT_SHADER, sources)
    {}

}
