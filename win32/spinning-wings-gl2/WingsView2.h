#pragma once

#include <Windows.h>
#include <GL/glew.h>

namespace silnith {

    /// <summary>
    /// Configures the OpenGL state machine for rendering the spinning wings animation.
    /// </summary>
    void InitializeOpenGLState2(void);

    /// <summary>
    /// Advances the spinning wings animation by one frame.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The caller must ensure that the current <c>HGLRC</c> is the same
    /// context that was initialized previously.
    /// </para>
    /// </remarks>
    void AdvanceAnimation2(void);

    /// <summary>
    /// Renders the current spinning wings animation frame into the current
    /// rendering context.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Be certain to call this with a valid device context and rendering context.
    /// In practice this means between calls to <c>BeginPaint</c> and <c>EndPaint</c>
    /// after receiving a message of type <c>WM_PAINT</c>.  Remember to also call
    /// <c>SwapBuffers</c> afterwards.
    /// </para>
    /// </remarks>
    void DrawFrame2(void);

    /// <summary>
    /// Updates the OpenGL rendering context for the new viewport size.
    /// </summary>
    /// <param name="width">the new viewport width</param>
    /// <param name="height">the new viewport height</param>
    void Resize2(GLsizei width, GLsizei height);

};
