/*
* TODO: Disable PCA in manifest.
* mark as DPI-aware
*/

#include <GL/glew.h>

#include <Windows.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <cassert>
#include <stdexcept>

#include "WingsPixelFormat.h"
#include "WingsViewGL2.h"

#include "resource.h"

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
std::unique_ptr<silnith::wings::gl2::WingsViewGL2> wingsView{ nullptr };

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HDC const hdc{ GetDC(hWnd) };

		int const pixelformat{ ChoosePixelFormat(hdc, &silnith::gl::desiredPixelFormat) };
		if (pixelformat == 0) {
			DWORD const error{ GetLastError() };
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
		if (hglrc == nullptr) {
			DWORD const error{ GetLastError() };
			ReleaseDC(hWnd, hdc);
			PostQuitMessage(-1);
			return -1;
		}

		BOOL const didMakeCurrent{ wglMakeCurrent(hdc, hglrc) };
		if (didMakeCurrent)
		{
			GLenum const glewInitError{ glewInit() };
			assert(glewInitError == GLEW_OK);
		}
		else
		{
			DWORD const error{ GetLastError() };
			ReleaseDC(hWnd, hdc);
			wglDeleteContext(hglrc);
			PostQuitMessage(-1);
			return -1;
		}

		try
		{
			wingsView = std::make_unique<silnith::wings::gl2::WingsViewGL2>(silnith::wings::gl::GLInfo{});
		}
		catch ([[maybe_unused]] std::exception const& e)
		{
			ReleaseDC(hWnd, hdc);
			wglDeleteContext(hglrc);
			PostQuitMessage(-1);
			return -1;
		}

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
	case WM_WINDOWPOSCHANGED:
	{
		WINDOWPOS const* windowPos{ reinterpret_cast<WINDOWPOS*>(lParam) };

		GLsizei const width{ static_cast<GLsizei>(windowPos->cx) };
		GLsizei const height{ static_cast<GLsizei>(windowPos->cy) };

		assert(hglrc == wglGetCurrentContext());

		wingsView->Resize(width, height);

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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		wingsView->DrawFrame();

		PAINTSTRUCT paintstruct{};
		HDC const hdc{ BeginPaint(hWnd, &paintstruct) };
		if (hdc == nullptr) {
			return -1;
		}

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
		wingsView.release();

		// window about to be destroyed
		HDC const hdc{ GetDC(hWnd) };
		wglMakeCurrent(hdc, nullptr);
		ReleaseDC(hWnd, hdc);

		wglDeleteContext(hglrc);

		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	}

	return 0;
}

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// register the window class for the main window

	HICON const hIcon{ static_cast<HICON>(LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_WINGS), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED)) };
	HCURSOR const hCursor{ static_cast<HCURSOR>(LoadImageW(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED)) };
	HBRUSH const hBrush{ nullptr };
	LPCWSTR const menuName{ nullptr };
	LPCWSTR const className{ L"SpinningWingsGL2" };
	int const iconSmWidth{ GetSystemMetrics(SM_CXSMICON) };
	int const iconSmHeight{ GetSystemMetrics(SM_CYSMICON) };
	HICON const hIconSm{ static_cast<HICON>(LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_WINGS), IMAGE_ICON, iconSmWidth, iconSmHeight, LR_DEFAULTCOLOR | LR_SHARED)) };

	WNDCLASSEXW const wndClassEx{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = hIcon,
		.hCursor = hCursor,
		.hbrBackground = hBrush,
		.lpszMenuName = menuName,
		.lpszClassName = className,
		.hIconSm = hIconSm,
	};
	ATOM const wndClassIdentifier{ RegisterClassExW(&wndClassEx) };
	if (wndClassIdentifier == 0)
	{
		return FALSE;
	}

	// create the main window

	DWORD const extendedWindowStyle{ WS_EX_APPWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_WINDOWEDGE };
	LPCWSTR const classType{ reinterpret_cast<LPCWSTR>(wndClassIdentifier) };
	LPCWSTR const windowName{ L"Spinning Wings GL2" };
	DWORD const windowStyle{ WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS };
	int const x{ CW_USEDEFAULT };
	int const y{ CW_USEDEFAULT };
	int const width{ CW_USEDEFAULT };
	int const height{ 600 };
	HWND const windowParent{ nullptr };
	HMENU const menu{ nullptr };
	HWND const window{ CreateWindowExW(extendedWindowStyle, classType, windowName, windowStyle, x, y, width, height, windowParent, menu, hInstance, nullptr) };
	if (window == nullptr) {
		// call GetLastError
		return FALSE;
	}

	// show the window

	ShowWindow(window, nShowCmd);
	UpdateWindow(window);

	// start the message loop

	MSG msg{};
	BOOL hasMessage{ GetMessageW(&msg, nullptr, 0, 0) };
	while (hasMessage) {
		if (hasMessage == -1) {
			return -1;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		hasMessage = GetMessageW(&msg, nullptr, 0, 0);
	}

	return (int)msg.wParam;
}
