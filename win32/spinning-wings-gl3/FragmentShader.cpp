#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

#include "FragmentShader.h"

namespace silnith::wings::gl3
{

    FragmentShader::FragmentShader(std::initializer_list<std::string> sources)
        : Shader{ GL_FRAGMENT_SHADER, sources }
    {}

}
