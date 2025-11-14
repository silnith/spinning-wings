#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <string>

#include "FragmentShader.h"
#include "VertexShader.h"

namespace silnith::wings::gl2
{
    /// <summary>
    /// Manages a GLSL program.  This is composed of shaders.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
    /// </para>
    /// </remarks>
    class Program
    {
    public:
        Program(void) noexcept = delete;
        Program(Program const&) noexcept = delete;
        Program& operator=(Program const&) noexcept = delete;
        Program(Program&&) noexcept = delete;
        Program& operator=(Program&&) noexcept = delete;
        virtual ~Program(void) noexcept;
    public:
        /// <summary>
        /// Creates and links a program from a given vertex and fragment shader.
        /// </summary>
        /// <param name="vertexShader">The vertex shader.</param>
        /// <param name="fragmentShader">The fragment shader.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(VertexShader const& vertexShader, FragmentShader const& fragmentShader);

        /// <summary>
        /// Creates and links a program from the provided vertex and fragment shaders.
        /// </summary>
        /// <param name="vertexShaders">The vertex shaders.  Only one may define the <c>main</c> function.</param>
        /// <param name="fragmentShaders">The fragment shaders.  Only one may define the <c>main</c> function.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
            std::initializer_list<std::shared_ptr<FragmentShader const> > fragmentShaders);

        /// <summary>
        /// Returns the OpenGL name for the program.
        /// </summary>
        /// <returns>The OpenGL identifier for the program.</returns>
        [[nodiscard]]
        GLuint getProgram(void) const noexcept;

        /// <summary>
        /// Returns the index of the generic vertex attribute that is bound to the
        /// specified attribute variable.
        /// </summary>
        /// <param name="name">The name of the attribute.</param>
        /// <returns>The generic attribute index.</returns>
        /// <exception cref="std::runtime_error">If the attribute name is not bound.</exception>
        [[nodiscard]]
        GLuint getAttributeLocation(std::string const& name) const;

        /// <summary>
        /// Installs the GLSL program as part of the current rendering state.
        /// </summary>
        /// <exception cref="std::runtime_error">If the program could not be installed.</exception>
        void useProgram(void) const;

    private:
        GLuint const id{ 0 };
        std::string linkLog{};
    };

}
