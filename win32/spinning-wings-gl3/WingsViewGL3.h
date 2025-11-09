#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>

#include "CurveGenerator.h"
#include "WingGL3.h"
#include "WingGeometry.h"
#include "WingRenderProgram.h"
#include "WingTransformProgram.h"

namespace silnith::wings::gl3
{

    /// <summary>
    /// An object to encapsulate all of the logic and data for initializing,
    /// animating, and rendering the 3D view of the spinning wings.
    /// </summary>
    /// <remarks>
    /// <para>
    /// This requires that the OpenGL state machine already be initialized
    /// and ready for use.  It should be destroyed before the GL rendering
    /// context is released.
    /// </para>
    /// <para>
    /// The OpenGL rendering context should support the OpenGL 3.2 Core profile.
    /// </para>
    /// </remarks>
    class WingsViewGL3
    {
    public:
        /// <summary>
        /// Configures the OpenGL state machine for rendering the spinning wings animation.
        /// </summary>
        WingsViewGL3(void);
        WingsViewGL3(WingsViewGL3 const&) = delete;
        WingsViewGL3& operator=(WingsViewGL3 const&) = delete;
        WingsViewGL3(WingsViewGL3&&) noexcept = delete;
        WingsViewGL3& operator=(WingsViewGL3&&) noexcept = delete;
        /// <summary>
        /// Cleans up any resources allocated.
        /// </summary>
        ~WingsViewGL3(void) noexcept = default;

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
        /// <summary>
        /// The number of wings to animate.
        /// </summary>
        std::size_t const numWings{ 40 };

        /// <summary>
        /// The sequence of transformed wings.
        /// </summary>
        std::deque<Wing<GLfloat> > wings{};

        /// <summary>
        /// The curve generator for the distance of the wing from the central axis.
        /// </summary>
        CurveGenerator<GLfloat> radiusCurve{ 10.0f, -15.0f, 15.0f, false, 0.1f, 0.01f, 150 };

        /// <summary>
        /// The curve generator for the angle that the wing is rotated around the central axis.
        /// </summary>
        CurveGenerator<GLfloat> angleCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 2.0f, 0.05f, 120) };

        /// <summary>
        /// The curve generator for the additional angle added to each successive "shadow" of the wing.
        /// </summary>
        CurveGenerator<GLfloat> deltaAngleCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(15.0f, 0.2f, 0.02f, 80) };

        /// <summary>
        /// The curve generator for the distance "up" the central axis that each wing shadow is moved.
        /// </summary>
        CurveGenerator<GLfloat> deltaZCurve{ 0.5f, 0.4f, 0.7f, false, 0.01f, 0.001f, 200 };

        /// <summary>
        /// The curve generator for the roll of the wing.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Roll, pitch, and yaw taken together define how the wing is twisted
        /// "in place" wherever it is around the central axis.
        /// </para>
        /// </remarks>
        CurveGenerator<GLfloat> rollCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 1.0f, 0.25f, 80) };

        /// <summary>
        /// The curve generator for the pitch of the wing.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Roll, pitch, and yaw taken together define how the wing is twisted
        /// "in place" wherever it is around the central axis.
        /// </para>
        /// </remarks>
        CurveGenerator<GLfloat> pitchCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 2.0f, 0.25f, 40) };

        /// <summary>
        /// The curve generator for the yaw of the wing.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Roll, pitch, and yaw taken together define how the wing is twisted
        /// "in place" wherever it is around the central axis.
        /// </para>
        /// </remarks>
        CurveGenerator<GLfloat> yawCurve{ CurveGenerator<GLfloat>::createGeneratorForAngles(0.0f, 1.5f, 0.25f, 50) };

        /// <summary>
        /// The curve generator for the red component of the wing color.
        /// </summary>
        CurveGenerator<GLfloat> redCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 95) };

        /// <summary>
        /// The curve generator for the green component of the wing color.
        /// </summary>
        CurveGenerator<GLfloat> greenCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 40) };

        /// <summary>
        /// The curve generator for the blue component of the wing color.
        /// </summary>
        CurveGenerator<GLfloat> blueCurve{ CurveGenerator<GLfloat>::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 70) };

        /// <summary>
        /// The various buffers that hold the wing geometry.
        /// </summary>
        std::shared_ptr<WingGeometry> wingGeometry{ nullptr };

        /// <summary>
        /// The GLSL program for transforming each wing and capturing the
        /// transformed geometry using transform feedback.
        /// </summary>
        std::unique_ptr<WingTransformProgram> wingTransformProgram{ nullptr };

        /// <summary>
        /// The GLSL program for rendering the wings.
        /// </summary>
        std::unique_ptr<WingRenderProgram> wingRenderProgram{ nullptr };
    };

}
