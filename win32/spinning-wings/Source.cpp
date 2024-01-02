/*
* TODO: Disable PCA in manifest.
* mark as DPI-aware
*/

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <cassert>

#include "WingsPixelFormat.h"
#include "WingsView.h"

#include "resource.h"

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

void CALLBACK TimerProc(HWND hWnd, UINT message, UINT_PTR timerId, DWORD currentTime)
{
	assert(timerId == animationTimerId);

	assert(hglrc == wglGetCurrentContext());

	silnith::wings::gl::AdvanceAnimation();

	InvalidateRgn(hWnd, nullptr, FALSE);
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
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		WORD const width{ LOWORD(lParam) };
		WORD const height{ HIWORD(lParam) };

		assert(hglrc == wglGetCurrentContext());

		silnith::wings::gl::Resize(static_cast<GLsizei>(width), static_cast<GLsizei>(height));

		return 0;
	}
	case WM_TIMER:
	{
		UINT_PTR const timerId{ static_cast<UINT_PTR>(wParam) };
		TIMERPROC const timerProc{ reinterpret_cast<TIMERPROC>(lParam) };

		assert(timerId == animationTimerId);
		assert(timerProc == nullptr);

		assert(hglrc == wglGetCurrentContext());

		silnith::wings::gl::AdvanceAnimation();

		InvalidateRgn(hWnd, nullptr, FALSE);
		return 0;
	}
	case WM_ERASEBKGND:
	{
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
	HBRUSH const hBrush{ GetSysColorBrush(COLOR_WINDOW) };
	LPCWSTR const menuName{ nullptr };
	LPCWSTR const className{ L"SpinningWings" };
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
	LPCWSTR const windowName{ L"Spinning Wings" };
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

	UINT_PTR const timerSet{ SetTimer(window, animationTimerId, updateDelayMilliseconds, nullptr) };

	assert(timerSet != 0);

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

	KillTimer(window, animationTimerId);

	return (int)msg.wParam;
}
