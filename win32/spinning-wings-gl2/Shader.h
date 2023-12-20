#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <initializer_list>
#include <string>

namespace silnith::wings::gl2
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
        Shader(Shader const&) noexcept = delete;
        Shader& operator=(Shader const&) noexcept = delete;
        Shader(Shader&&) noexcept = delete;
        Shader& operator=(Shader&&) noexcept = delete;
        virtual ~Shader(void) noexcept;
    protected:
        /// <summary>
        /// Creates and compiles a shader from the given GLSL sources.
        /// The source strings are concatenated with newlines appended to each.
        /// </summary>
        /// <param name="type">The type of shader.  This should be one of <c>GL_VERTEX_SHADER</c>, <c>GL_GEOMETRY_SHADER</c>, or <c>GL_FRAGMENT_SHADER</c>.</param>
        /// <param name="sources">The source strings to concatenate.</param>
        explicit Shader(GLenum type, std::initializer_list<std::string> const& sources);

    public:
        GLuint getShader(void) const noexcept;

    private:
        GLuint id{ 0 };
        std::string compilationLog{};
    };

}
