#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

namespace silnith::wings::gl4
{
    /// <summary>
    /// Manages a single GLSL shader.  Use <c>FragmentShader</c>
    /// or <c>VertexShader</c> instead.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This is designed for OpenGL 4.1 or greater.
    /// </para>
    /// </remarks>
    class Shader
    {
    public:
        Shader(void) noexcept = delete;
        Shader(Shader const&) noexcept = delete;
        Shader& operator=(Shader const&) noexcept = delete;
        Shader(Shader&&) noexcept = delete;
        Shader& operator=(Shader&&) noexcept = delete;
        virtual ~Shader(void) noexcept;
    protected:
        explicit Shader(GLenum type, std::initializer_list<std::string> const&);

    public:
        GLuint getShader(void) const noexcept;

    private:
        GLuint id{ 0 };
        std::string compilationLog{};
    };

}
