#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <string>
#include <vector>

namespace silnith::gl2
{
    /// <summary>
    /// Manages a single GLSL shader.  Use <c>FragmentShader</c>
    /// or <c>VertexShader</c> instead.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
    /// </para>
    /// </remarks>
    class Shader
    {
    public:
        Shader(void) noexcept = delete;
        ~Shader(void) noexcept;
        Shader(Shader const&) noexcept = delete;
        Shader& operator=(Shader const&) noexcept = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

    protected:
        Shader(GLenum type, std::vector<std::string> const &);

    public:
        GLuint getShader(void) const noexcept;

    private:
        GLuint id{ 0 };
        std::string compilationLog{};
    };

};
