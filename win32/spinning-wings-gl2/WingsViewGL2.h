#pragma once

#include <Windows.h>
#include <GL/glew.h>

#include <deque>
#include <memory>

#include <cstddef>

#include "CurveGenerator.h"
#include "GLInfo.h"
#include "WingRenderer.h"
#include "Wing.h"

#include "FragmentShader.h"
#include "Program.h"
#include "VertexShader.h"

namespace silnith::wings::gl2
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
    /// </remarks>
    class WingsViewGL2
    {
#pragma region Rule of Five

    public:
        WingsViewGL2(WingsViewGL2 const&) = delete;
        WingsViewGL2& operator=(WingsViewGL2 const&) = delete;
        WingsViewGL2(WingsViewGL2&&) noexcept = delete;
        WingsViewGL2& operator=(WingsViewGL2&&) noexcept = delete;
        ~WingsViewGL2(void) noexcept;

#pragma endregion

    public:
        /// <summary>
        /// Default constructor is deleted.  A <c>GLInfo</c> is required to initialize properly.
        /// </summary>
        WingsViewGL2(void) = delete;

        /// <summary>
        /// Configures the OpenGL state machine for rendering the spinning wings animation.
        /// </summary>
        /// <param name="glInfo">The queryable OpenGL information.</param>
        WingsViewGL2(silnith::wings::gl::GLInfo const& glInfo);

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
        /// <remarks>
        /// <para>
        /// This updates the viewport and adjusts the projection matrix to
        /// account for the aspect ratio.
        /// </para>
        /// </remarks>
        /// <param name="width">the new viewport width</param>
        /// <param name="height">the new viewport height</param>
        void Resize(GLsizei width, GLsizei height) const;

        /// <summary>
        /// Updates the OpenGL rendering context for the new viewport size.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This updates the viewport and adjusts the projection matrix to
        /// account for the aspect ratio.
        /// </para>
        /// </remarks>
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
        /// Whether the GL supports the polygon offset feature.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This feature prevents Z-fighting between the polygon surface and
        /// the wire outline of each wing.  If it is supported, then the wire
        /// outlines will be rendered.  The outlines will also have smoothing
        /// enabled, which requires fragment blending.
        /// </para>
        /// <para>
        /// Since this project is a demonstration for OpenGL 2.0, this will
        /// always be <see langword="true"/>.
        /// </para>
        /// </remarks>
        bool const enablePolygonOffset{ false };

        /// <summary>
        /// A handler for drawing the actual wing.
        /// </summary>
        /// <remarks>
        /// <para>
        /// The actual implementation will be chosen at runtime during
        /// construction based on the version of OpenGL supported.
        /// </para>
        /// <para>
        /// (Of course, since this is the OpenGL 2.x demonstration, the
        /// renderer chosen will always be the OpenGL 1.5 implementation.)
        /// </para>
        /// </remarks>
        std::unique_ptr<silnith::wings::gl::WingRenderer> wingRenderer{ nullptr };

        /// <summary>
        /// The sequence of transformed wings.
        /// </summary>
        std::deque<Wing<GLuint, GLfloat> > wings{};

#pragma region The Random Curve Generators

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

#pragma endregion

        /// <summary>
        /// The GLSL program for rendering.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This has three attributes that should be populated using <c>glGetAttribLocation</c>:
        /// </para>
        /// <list>
        /// <listheader>
        /// <term>Attribute</term>
        /// <description>Data Type</description>
        /// </listheader>
        /// <item>
        /// <term>deltaZ</term>
        /// <description>vec2</description>
        /// </item>
        /// <item>
        /// <term>radiusAngle</term>
        /// <description>vec2</description>
        /// </item>
        /// <item>
        /// <term>rollPitchYaw</term>
        /// <description>vec3</description>
        /// </item>
        /// </list>
        /// </remarks>
        /// <seealso cref="deltaZAttribLocation"/>
        /// <seealso cref="radiusAngleAttribLocation"/>
        /// <seealso cref="rollPitchYawAttribLocation"/>
        std::unique_ptr<Program> glslProgram{ nullptr };

        /// <summary>
        /// The attribute location for the <c>deltaZ</c> attribute.
        /// This is of type <c>vec2</c>.
        /// </summary>
        /// <example>
        /// <code>
        /// glVertexAttrib2f(deltaZAttribLocation, deltaAngle, deltaZ);
        /// </code>
        /// </example>
        /// <seealso cref="glslProgram"/>
        GLuint deltaZAttribLocation{ 0 };

        /// <summary>
        /// The attribute location for the <c>radiusAngle</c> attribute.
        /// This is of type <c>vec2</c>.
        /// </summary>
        /// <example>
        /// <code>
        /// glVertexAttrib2f(radiusAngleAttribLocation, radius, angle);
        /// </code>
        /// </example>
        /// <seealso cref="glslProgram"/>
        GLuint radiusAngleAttribLocation{ 0 };

        /// <summary>
        /// The attribute location for the <c>rollPitchYaw</c> attribute.
        /// This is of type <c>vec3</c>.
        /// </summary>
        /// <example>
        /// <code>
        /// glVertexAttrib3f(rollPitchYawAttribLocation, roll, pitch, yaw);
        /// </code>
        /// </example>
        /// <seealso cref="glslProgram"/>
        GLuint rollPitchYawAttribLocation{ 0 };
    };
}
