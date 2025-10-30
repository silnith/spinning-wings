#pragma once

#include <Windows.h>
#include <gl/GL.h>

#include <deque>

#include "CurveGenerator.h"
#include "GLInfo.h"
#include "Wing.h"

namespace silnith::wings::gl
{

    class WingsView
    {
    public:
        WingsView(void) = delete;
        WingsView(WingsView const&) = delete;
        WingsView& operator=(WingsView const&) = delete;
        WingsView(WingsView&&) noexcept = delete;
        WingsView& operator=(WingsView&&) noexcept = delete;
        ~WingsView(void) noexcept;

    public:
        WingsView(GLInfo const&);

    public:
        /// <summary>
        /// Advances the spinning wings animation by one frame.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The caller must ensure that the current <c>HGLRC</c> is the same
        /// context that was initialized previously.
        /// </para>
        /// </remarks>
        void AdvanceAnimation(void);

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
        void DrawFrame(void) const;

        /// <summary>
        /// Updates the OpenGL rendering context for the new viewport size.
        /// </summary>
        /// <param name="width">the new viewport width</param>
        /// <param name="height">the new viewport height</param>
        void Resize(GLsizei width, GLsizei height) const;

        /// <summary>
        /// Updates the OpenGL rendering context for the new viewport size.
        /// </summary>
        /// <param name="x">the new viewport starting X coordinate</param>
        /// <param name="y">the new viewport starting Y coordinate</param>
        /// <param name="width">the new viewport width</param>
        /// <param name="height">the new viewport height</param>
        void Resize(GLint x, GLint y, GLsizei width, GLsizei height) const;

    private:
        std::size_t const numWings{ 40 };
        bool const enablePolygonOffset{ false };
        GLuint const wingDisplayList{ 0 };
        std::deque<Wing<GLuint, GLfloat> > wings{};
        CurveGenerator<GLfloat> radiusCurve{ 10.0f, -15.0f, 15.0f, false, 0.1f, 0.01f, 150 };
        CurveGenerator<GLfloat> angleCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 2.0f, 0.05f, 120) };
        CurveGenerator<GLfloat> deltaAngleCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(15.0f, 0.2f, 0.02f, 80) };
        CurveGenerator<GLfloat> deltaZCurve{ 0.5f, 0.4f, 0.7f, false, 0.01f, 0.001f, 200 };
        CurveGenerator<GLfloat> rollCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 1.0f, 0.25f, 80) };
        CurveGenerator<GLfloat> pitchCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 2.0f, 0.25f, 40) };
        CurveGenerator<GLfloat> yawCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 1.5f, 0.25f, 50) };
        CurveGenerator<GLfloat> redCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 95) };
        CurveGenerator<GLfloat> greenCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 40) };
        CurveGenerator<GLfloat> blueCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 70) };
    };

}
