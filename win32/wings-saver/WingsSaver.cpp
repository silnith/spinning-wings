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

#include <memory>

#include <cassert>

#include "GLInfo.h"
#include "WingsPixelFormat.h"
#include "WingsView.h"

/// <summary>
/// The number of milliseconds between frame updates.
/// </summary>
/// <remarks>
/// <para>
/// Windows will clamp this value to the range
/// <c>[<see cref="USER_TIMER_MINIMUM"/>, <see cref="USER_TIMER_MAXIMUM"/>]</c>.
/// </para>
/// </remarks>
UINT constexpr updateDelayMilliseconds{ 33 };

/// <summary>
/// A randomly-chosen identifier for the animation timer.
/// </summary>
UINT_PTR constexpr animationTimerId{ 42 };

/// <summary>
/// The OpenGL rendering context.
/// </summary>
/// <remarks>
/// <para>
/// The Device Context (DC) is the Windows object that represents the drawable surface.
/// The OpenGL rendering context (GLRC) is the OpenGL state machine.
/// A thread has a current GLRC specified by <see cref="wglMakeCurrent"/>.
/// Each GLRC has an associated DC, but the DC is ignorant of the GLRC.
/// </para>
/// </remarks>
HGLRC hglrc{ nullptr };

/// <summary>
/// The object that encapsulates all of the logic for drawing and animating
/// the spinning wings.
/// </summary>
std::unique_ptr<silnith::wings::gl::WingsView> wingsView{ nullptr };

/// <summary>
/// The current width of the display window.
/// </summary>
/// <remarks>
/// <para>
/// This is updated after every <see cref="WM_WINDOWPOSCHANGED"/> message received.
/// </para>
/// </remarks>
GLsizei currentWindowWidth{ 0 };

/// <summary>
/// The current height of the display window.
/// </summary>
/// <remarks>
/// <para>
/// This is updated after every <see cref="WM_WINDOWPOSCHANGED"/> message received.
/// </para>
/// </remarks>
GLsizei currentWindowHeight{ 0 };

/// <summary>
/// Renders the current view to the portion of the window that is visible on a single monitor.
/// This sets the drawing area to the visible portion for the monitor, sets the GL scissor to discard
/// fragments outside of the visible portion, then draws the current frame.
/// </summary>
/// <remarks>
/// <para>
/// This is an instance of <see cref="MONITORENUMPROC"/> intended to be passed to
/// <see cref="EnumDisplayMonitors"/>.
/// </para>
/// </remarks>
/// <param name="hMonitor">A handle to the display monitor.  This value will always be non-<c>NULL</c>.</param>
/// <param name="hdcMonitor">A handle to a device context.  This may be <c>NULL</c>.</param>
/// <param name="lprcMonitor">The clipping rectangle of the device context portion that appears on this monitor.</param>
/// <param name="dwData">Whatever the fourth parameter to <see cref="EnumDisplayMonitors"/> was.</param>
/// <returns><c>TRUE</c> to continue the enumeration.</returns>
/// <seealso cref="MONITORENUMPROC"/>
/// <seealso cref="EnumDisplayMonitors"/>
BOOL CALLBACK RenderWingsOnMonitor(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	LONG const visibleTop{ lprcMonitor->top };
	LONG const visibleBottom{ lprcMonitor->bottom };
	LONG const visibleLeft{ lprcMonitor->left };
	LONG const visibleRight{ lprcMonitor->right };

	LONG const visibleWidth{ visibleRight - visibleLeft };
	LONG const visibleHeight{ visibleBottom - visibleTop };

	GLint const x{ visibleLeft };
	GLint const y{ currentWindowHeight - visibleBottom };

	GLsizei const width{ static_cast<GLsizei>(visibleWidth) };
	GLsizei const height{ static_cast<GLsizei>(visibleHeight) };

	wingsView->Resize(x, y, width, height);

	glScissor(x, y, width, height);

	wingsView->DrawFrame();

	return TRUE;
}

/// <summary>
/// The <see cref="TIMERPROC"/> that advances the animation by one frame.
/// </summary>
/// <remarks>
/// <para>
/// Pass this to <see cref="SetTimer"/>.
/// </para>
/// </remarks>
/// <param name="hWnd">A handle to the window associated with the timer.</param>
/// <param name="uMsg">The message code.  Must be <see cref="WM_TIMER"/>.</param>
/// <param name="idEvent">The timer identifier.  Should be <see cref="animationTimerId"/>.</param>
/// <param name="dwTime">The number of milliseconds that have elapsed since the system was started.
/// This is the value returned by the <see cref="GetTickCount"/> function.</param>
/// <seealso cref="TIMERPROC"/>
/// <seealso cref="SetTimer"/>
/// <seealso cref="StartAnimation"/>
/// <seealso cref="StopAnimation"/>
void CALLBACK AdvanceAnimation(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	assert(uMsg == WM_TIMER);
	assert(idEvent == animationTimerId);

	assert(hglrc == wglGetCurrentContext());

	wingsView->AdvanceAnimation();

	HRGN constexpr hRegion{ nullptr };
	BOOL constexpr eraseBackground{ FALSE };
	InvalidateRgn(hWnd, hRegion, eraseBackground);
}

/// <summary>
/// Begins a timer that calls <see cref="AdvanceAnimation"/> every <see cref="updateDelayMilliseconds"/> milliseconds.
/// </summary>
/// <param name="hWnd">The window handle.  This is required for the timer.</param>
/// <seealso cref="animationTimerId"/>
/// <seealso cref="StopAnimation"/>
/// <seealso cref="SetTimer"/>
void StartAnimation(HWND hWnd)
{
	TIMERPROC constexpr timerProc{ AdvanceAnimation };
	UINT_PTR const timerSet{ SetTimer(hWnd, animationTimerId, updateDelayMilliseconds, timerProc) };

	assert(timerSet != 0);

	if (timerSet == 0)
	{
		DWORD const error{ GetLastError() };
	}
}

/// <summary>
/// Ends the timer that calls <see cref="AdvanceAnimation"/>.
/// </summary>
/// <param name="hWnd">The window handle.</param>
/// <seealso cref="animationTimerId"/>
/// <seealso cref="StartAnimation"/>
/// <seealso cref="KillTimer"/>
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
		/*
		 * This is to disable the "helpful" exception handler that Windows puts around timers, starting with Windows 2000.
		 * They added it, then immediately realized it was a terrible idea and a security vulnerability,
		 * but kept it for "compatibility" and instead told everybody to change their code to disable it instead.
		 */
		HANDLE const processHandle{ GetCurrentProcess() };
		BOOL suppressExceptions{ FALSE };
		PVOID const buffer_address{ &suppressExceptions };
		DWORD constexpr buffer_length{ sizeof(suppressExceptions) };
		BOOL const exceptionHandlerDisabled{ SetUserObjectInformationW(processHandle, UOI_TIMERPROC_EXCEPTION_SUPPRESSION, buffer_address, buffer_length) };

		if (exceptionHandlerDisabled) {}
		else
		{
			DWORD const error{ GetLastError() };
		}

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

		wingsView = std::make_unique<silnith::wings::gl::WingsView>(silnith::wings::gl::GLInfo{});

		ReleaseDC(hWnd, hdc);

		StartAnimation(hWnd);

		return 0;
	}
	case WM_DISPLAYCHANGE:
	{
		// TODO: monitors changed?
		return DefWindowProcW(hWnd, message, wParam, lParam);
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
	case WM_WINDOWPOSCHANGED:
	{
		WINDOWPOS const* windowPos{ reinterpret_cast<WINDOWPOS*>(lParam) };

		currentWindowWidth = static_cast<GLsizei>(windowPos->cx);
		currentWindowHeight = static_cast<GLsizei>(windowPos->cy);

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

		glDisable(GL_SCISSOR_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_SCISSOR_TEST);

		PAINTSTRUCT paintstruct{};
		HDC const hdc{ BeginPaint(hWnd, &paintstruct) };
		if (hdc == nullptr) {
			return -1;
		}
		LPCRECT constexpr lprcClip{ nullptr };
		MONITORENUMPROC constexpr lpfnEnum{ RenderWingsOnMonitor };
		LPARAM constexpr dwData{ 0 };
		EnumDisplayMonitors(hdc, lprcClip, lpfnEnum, dwData);

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
		wingsView.reset();

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
