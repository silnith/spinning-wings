#pragma once

#include <Windows.h>

namespace silnith::gl
{

    /// <summary>
    /// A descriptor for a pixel format that supports OpenGL rendering
    /// using RGBA and depth buffers to a double-buffered window.
    /// </summary>
    extern PIXELFORMATDESCRIPTOR constexpr desiredPixelFormat{
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
        .cRedBits = 8,
        .cRedShift = 0,
        .cGreenBits = 8,
        .cGreenShift = 0,
        .cBlueBits = 8,
        .cBlueShift = 0,
        .cAlphaBits = 0,
        .cAlphaShift = 0,
        .cAccumBits = 0,
        .cAccumRedBits = 0,
        .cAccumGreenBits = 0,
        .cAccumBlueBits = 0,
        .cAccumAlphaBits = 0,
        .cDepthBits = 32,
        .cStencilBits = 0,
        .cAuxBuffers = 0,
        .iLayerType = PFD_MAIN_PLANE,
        .bReserved = 0,
        .dwLayerMask = 0,
        .dwVisibleMask = 0,
        .dwDamageMask = 0,
    };

}
