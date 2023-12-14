/*
* TODO: Disable PCA in manifest.
* mark as DPI-aware
*/

#include <GL/glew.h>
#include <GL/wglew.h>

#include <Windows.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <cassert>
#include <stdexcept>

#include "WingsPixelFormat.h"

#include "WingsViewGL3.h"

UINT const updateDelayMilliseconds{ 33 };

/*
 * The Device Context (DC) is the Windows object that represents the drawable surface.
 * The OpenGL rendering context (GLRC) is the OpenGL state machine.
 * A thread has a current GLRC specified by wglMakeCurrent.
 * Each GLRC has an associated DC, but the DC is ignorant of the GLRC.
 */

HGLRC hglrc{};

BOOL MonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT lpRect, LPARAM d)
{
	return TRUE;
}

void TimerProc(HWND hWnd, UINT message, UINT_PTR bar, DWORD baz)
{
	silnith::wings::gl3::AdvanceAnimation();
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
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		//int foo = DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		BOOL const didSetPixelFormat{ SetPixelFormat(hdc, pixelformat, &silnith::gl::desiredPixelFormat) };
		if (didSetPixelFormat) {}
		else
		{
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		HGLRC tempGLRC{ wglCreateContext(hdc) };
		if (tempGLRC == NULL) {
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		BOOL const didMakeCurrent{ wglMakeCurrent(hdc, tempGLRC) };
		if (didMakeCurrent)
		{
			GLenum const glewInitError{ glewInit() };
			assert(glewInitError == GLEW_OK);

			int const attribList[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
				WGL_CONTEXT_MINOR_VERSION_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
				WGL_CONTEXT_LAYER_PLANE_ARB, 0,
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
				0,
			};

			hglrc = wglCreateContextAttribsARB(hdc, tempGLRC, attribList);
			wglMakeCurrent(hdc, hglrc);
			wglDeleteContext(tempGLRC);
		}
		else
		{
			wglDeleteContext(tempGLRC);
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		try
		{
			silnith::wings::gl3::InitializeOpenGLState();
		}
		catch ([[maybe_unused]] std::exception const& e)
		{
			wglDeleteContext(hglrc);
			PostQuitMessage(-1);
			return -1;
		}

		ReleaseDC(hWnd, hdc);

		return 0;
	}
	case WM_DPICHANGED:
	{
		// GetSystemMetricsForDpi, AdjustWindowRectExForDpi, SystemParametersInfoForDpi, GetDpiForWindow
		WORD const yAxisDPI{ HIWORD(wParam) };
		WORD const xAxisDPI{ LOWORD(wParam) };
		LPRECT const suggestedSizeAndPosition{ (RECT*)lParam };
		/*
		* TODO: Guard this call to Windows 8.1 and later.
		*/
		BOOL const success{ SetWindowPos(hWnd, NULL, suggestedSizeAndPosition->left, suggestedSizeAndPosition->top, suggestedSizeAndPosition->right - suggestedSizeAndPosition->left, suggestedSizeAndPosition->bottom - suggestedSizeAndPosition->top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS) };
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

		HDC const hdc{ GetDC(hWnd) };

		GLsizei const width{ LOWORD(lParam) };
		GLsizei const height{ HIWORD(lParam) };
		silnith::wings::gl3::Resize(width, height);

		ReleaseDC(hWnd, hdc);

		return 0;
	}
	case WM_TIMER:
	{
		silnith::wings::gl3::AdvanceAnimation();

		InvalidateRgn(hWnd, NULL, FALSE);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT paintstruct;
		HDC const hdc{ BeginPaint(hWnd, &paintstruct) };
		if (hdc == NULL) {
			return -1;
		}
		//EnumDisplayMonitors(hdc, NULL, MonitorEnumProc, 0);

		silnith::wings::gl3::DrawFrame();

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
		silnith::wings::gl3::CleanupOpenGLState();

		// window about to be destroyed
		HDC const hdc{ GetDC(hWnd) };
		wglMakeCurrent(hdc, NULL);
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

	WNDCLASSEXW const wndClassEx{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = NULL,  // LoadIcon
		.hCursor = NULL,  // LoadCursor
		// GetStockObject(BLACK_BRUSH);
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = NULL,
		.lpszClassName = L"Project1Class",
		.hIconSm = NULL,
	};
	ATOM const wndClassIdentifier{ RegisterClassExW(&wndClassEx) };
	if (wndClassIdentifier == 0)
	{
		return FALSE;
	}

	// create the main window

	DWORD const extendedWindowStyle{ WS_EX_APPWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_WINDOWEDGE };
	LPCWSTR const className{ (LPCWSTR)wndClassIdentifier };
	LPCWSTR const windowName{ L"Project1Window" };
	DWORD const windowStyle{ WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS };
	int const x{ CW_USEDEFAULT };
	int const y{ CW_USEDEFAULT };
	int const width{ CW_USEDEFAULT };
	int const height{ 600 };
	HWND const windowParent{ NULL };
	HMENU const menu{ NULL };
	HWND const window{ CreateWindowExW(extendedWindowStyle, className, windowName, windowStyle, x, y, width, height, windowParent, menu, hInstance, NULL) };
	if (window == NULL) {
		// call GetLastError
		return FALSE;
	}

	// show the window

	ShowWindow(window, nShowCmd);
	UpdateWindow(window);

	UINT_PTR const timer{ SetTimer(window, 42, updateDelayMilliseconds, NULL) };

	// start the message loop

	MSG msg{};
	BOOL hasMessage{ GetMessageW(&msg, NULL, 0, 0) };
	while (hasMessage) {
		if (hasMessage == -1) {
			return -1;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		hasMessage = GetMessageW(&msg, NULL, 0, 0);
	}

	KillTimer(window, timer);

	return (int)msg.wParam;
}
