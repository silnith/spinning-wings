#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

#include "VertexShader.h"

namespace silnith::wings::gl2
{

    std::shared_ptr<VertexShader const> VertexShader::MakeRotateMatrixShader(void)
    {
        return std::make_shared<VertexShader const>(std::initializer_list<std::string>{
            Shader::versionDeclaration,
            Shader::rotateMatrixFunctionDefinition,
        });
    }

    std::shared_ptr<VertexShader const> VertexShader::MakeTranslateMatrixShader(void)
    {
        return std::make_shared<VertexShader const>(std::initializer_list<std::string>{
            Shader::versionDeclaration,
            Shader::translateMatrixFunctionDefinition,
        });
    }

    std::shared_ptr<VertexShader const> VertexShader::MakeScaleMatrixShader(void)
    {
        return std::make_shared<VertexShader const>(std::initializer_list<std::string>{
            Shader::versionDeclaration,
            Shader::scaleMatrixFunctionDefinition,
        });
    }

    VertexShader::VertexShader(std::initializer_list<std::string> sources)
        : Shader{ GL_VERTEX_SHADER, sources }
    {}

}
