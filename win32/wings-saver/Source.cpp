#include <Windows.h>
#include <ScrnSave.h>

#ifdef UNICODE
#pragma comment (lib, "scrnsavw.lib")
#else
#pragma comment (lib, "scrnsave.lib")
#endif
#pragma comment (lib, "comctl32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <cassert>

#include "WingsPixelFormat.h"
#include "WingsView.h"

/// <summary>
/// The number of milliseconds between frame updates.
/// </summary>
UINT constexpr updateDelayMilliseconds{ 33 };

UINT_PTR constexpr animationTimerId{ 42 };

/*
 * The Device Context (DC) is the Windows object that represents the drawable surface.
 * The OpenGL rendering context (GLRC) is the OpenGL state machine.
 * A thread has a current GLRC specified by wglMakeCurrent.
 * Each GLRC has an associated DC, but the DC is ignorant of the GLRC.
 */

HGLRC hglrc{ nullptr };

BOOL MonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT lpRect, LPARAM d)
{
	return TRUE;
}

/// <summary>
/// The <c>TIMERPROC</c> that advances the animation by one frame.
/// </summary>
/// <remarks>
/// <para>
/// Pass this to <c>SetTimer</c>.
/// </para>
/// </remarks>
/// <param name="hWnd">The window handle.</param>
/// <param name="message">The message code.  Must be <c>WM_TIMER</c>.</param>
/// <param name="timerId">The timer identifier.  Should be <c>animationTimerId</c>.</param>
/// <param name="tickCount">The current tick count.</param>
void CALLBACK AdvanceAnimation(HWND hWnd, UINT message, UINT_PTR timerId, DWORD tickCount)
{
	assert(message == WM_TIMER);
	assert(timerId == animationTimerId);

	assert(hglrc == wglGetCurrentContext());

	silnith::wings::gl::AdvanceAnimation();

	HRGN constexpr hRegion{ nullptr };
	BOOL constexpr eraseBackground{ FALSE };
	InvalidateRgn(hWnd, hRegion, eraseBackground);
}

void StartAnimation(HWND hWnd)
{
	TIMERPROC constexpr timerProc{ AdvanceAnimation };
	UINT_PTR const timerSet{ SetTimer(hWnd, animationTimerId, updateDelayMilliseconds, timerProc) };

	assert(timerSet != 0);
}

void StopAnimation(HWND hWnd)
{
	BOOL const timerStopped{ KillTimer(hWnd, animationTimerId) };

	assert(timerStopped);
}

// add to EXPORTS statement in module-definition (.def) file
LRESULT WINAPI ScreenSaverProcW(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HDC const hdc{ GetDC(hWnd) };

		int const pixelformat{ ChoosePixelFormat(hdc, &silnith::gl::desiredPixelFormat) };
		if (pixelformat == 0) {
			DWORD error{ GetLastError() };
			ReleaseDC(hWnd, hdc);
			PostQuitMessage(-1);
			return -1;
		}

		//int foo = DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		BOOL const didSetPixelFormat{ SetPixelFormat(hdc, pixelformat, &silnith::gl::desiredPixelFormat) };
		if (didSetPixelFormat) {}
		else
		{
			DWORD const error{ GetLastError() };
			ReleaseDC(hWnd, hdc);
			PostQuitMessage(-1);
			return -1;
		}

		hglrc = wglCreateContext(hdc);
		if (hglrc == NULL) {
			DWORD const error{ GetLastError() };
			ReleaseDC(hWnd, hdc);
			PostQuitMessage(-1);
			return -1;
		}

		BOOL const didMakeCurrent{ wglMakeCurrent(hdc, hglrc) };
		if (didMakeCurrent) {}
		else
		{
			DWORD const error{ GetLastError() };
			ReleaseDC(hWnd, hdc);
			wglDeleteContext(hglrc);
			PostQuitMessage(-1);
			return -1;
		}

		silnith::wings::gl::InitializeOpenGLState();

		ReleaseDC(hWnd, hdc);

		StartAnimation(hWnd);

		return 0;
	}
	case WM_DPICHANGED:
	{
		// GetSystemMetricsForDpi, AdjustWindowRectExForDpi, SystemParametersInfoForDpi, GetDpiForWindow
		WORD const yAxisDPI{ HIWORD(wParam) };
		WORD const xAxisDPI{ LOWORD(wParam) };
		LPRECT const suggestedSizeAndPosition{ reinterpret_cast<LPRECT>(lParam) };
		/*
		* TODO: Guard this call to Windows 8.1 and later.
		*/
		BOOL const success{ SetWindowPos(hWnd, nullptr, suggestedSizeAndPosition->left, suggestedSizeAndPosition->top, suggestedSizeAndPosition->right - suggestedSizeAndPosition->left, suggestedSizeAndPosition->bottom - suggestedSizeAndPosition->top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS) };
		return 0;
	}
	case WM_SIZE:
	{
		switch (wParam)
		{
		case SIZE_MAXHIDE:
		case SIZE_MAXSHOW:
		case SIZE_MAXIMIZED:
		case SIZE_RESTORED:
			break;
		case SIZE_MINIMIZED:
		default:
			return DefScreenSaverProc(hWnd, message, wParam, lParam);
		}

		assert(hglrc == wglGetCurrentContext());

		GLsizei const width{ LOWORD(lParam) };
		GLsizei const height{ HIWORD(lParam) };
		silnith::wings::gl::Resize(width, height);

		return 0;
	}
	case WM_PAINT:
	{
		/*
		 * If I understand the Win32 documentation correctly, I do not need to
		 * own the DC in order to issue OpenGL commands to the GL rendering
		 * context for the current thread.  But I should own it in order to
		 * issue the SwapBuffers call, because that affects the DC.
		 *
		 * TODO: Is this only for double-buffered windows?
		 */
		assert(hglrc == wglGetCurrentContext());

		silnith::wings::gl::DrawFrame();

		PAINTSTRUCT paintstruct{};
		HDC const hdc{ BeginPaint(hWnd, &paintstruct) };
		if (hdc == nullptr) {
			return -1;
		}
		//EnumDisplayMonitors(hdc, nullptr, MonitorEnumProc, 0);

		//wglMakeCurrent(hdc, hglrc);

		//silnith::wings::gl::DrawFrame();

		SwapBuffers(hdc);

		EndPaint(hWnd, &paintstruct);
		return 0;
	}
	case WM_CLOSE:
	{
		StopAnimation(hWnd);

		BOOL const destroyed{ DestroyWindow(hWnd) };
		return 0;
	}
	case WM_DESTROY:
	{
		silnith::wings::gl::CleanupOpenGLState();

		// window about to be destroyed
		HDC const hdc{ GetDC(hWnd) };
		wglMakeCurrent(hdc, nullptr);
		ReleaseDC(hWnd, hdc);

		wglDeleteContext(hglrc);

		return DefScreenSaverProc(hWnd, message, wParam, lParam);
	}
	default:
	{
		return DefScreenSaverProc(hWnd, message, wParam, lParam);
	}
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}
