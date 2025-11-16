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
    /// The invariant established by this class is that the OpenGL program
    /// object is allocated and successfully linked.  The input shaders are
    /// only required for construction.  After the object is constructed, the
    /// shaders are no longer required.
    /// </para>
    /// <para>
    /// This requires OpenGL 2.0 or greater.
    /// </para>
    /// </remarks>
    class Program
    {
    public:
        Program(void) = delete;
        Program(Program const&) = delete;
        Program& operator=(Program const&) = delete;
        Program(Program&&) noexcept = delete;
        Program& operator=(Program&&) noexcept = delete;
        virtual ~Program(void) noexcept;

    public:
        /// <summary>
        /// Creates and links a program using the provided vertex and fragment shaders.
        /// </summary>
        /// <param name="vertexShaders">The vertex shaders.  Only one may define the <c>main</c> function.</param>
        /// <param name="fragmentShaders">The fragment shaders.  Only one may define the <c>main</c> function.</param>
        /// <exception cref="std::runtime_error">If the program fails to link.</exception>
        explicit Program(std::initializer_list<std::shared_ptr<VertexShader const> > vertexShaders,
            std::initializer_list<std::shared_ptr<FragmentShader const> > fragmentShaders);

        /// <summary>
        /// Returns the OpenGL name for the program object.
        /// </summary>
        /// <returns>The program object name.</returns>
        [[nodiscard]]
        GLuint GetName(void) const noexcept;

        /// <summary>
        /// Returns the index of the generic vertex attribute that is bound to the
        /// specified attribute variable.
        /// </summary>
        /// <param name="attributeName">The name of the attribute.</param>
        /// <returns>The generic attribute index.</returns>
        /// <exception cref="std::runtime_error">If the attribute name is not bound.</exception>
        [[nodiscard]]
        GLuint getAttributeLocation(std::string const& attributeName) const;

        /// <summary>
        /// Installs the GLSL program as part of the current rendering state.
        /// </summary>
        /// <exception cref="std::runtime_error">If the program could not be installed.</exception>
        void useProgram(void) const;

    private:
        /// <summary>
        /// The OpenGL name for the program object.
        /// </summary>
        GLuint const name{ 0 };

        /// <summary>
        /// The log output from linking the shaders into a program.
        /// </summary>
        std::string linkLog{};
    };

}
