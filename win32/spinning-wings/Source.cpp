/*
* TODO: Disable PCA in manifest.
* mark as DPI-aware
*/

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include "WingsView.h"
#include "WingsPixelFormat.h"

#include <cassert>

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
	silnith::AdvanceAnimation();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HDC const hdc{ GetDC(hWnd) };

		int const pixelformat{ ChoosePixelFormat(hdc, &silnith::desiredPixelFormat) };
		if (pixelformat == 0) {
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		//int foo = DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		BOOL const didSetPixelFormat{ SetPixelFormat(hdc, pixelformat, &silnith::desiredPixelFormat) };
		if (didSetPixelFormat) {}
		else
		{
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		hglrc = wglCreateContext(hdc);
		if (hglrc == NULL) {
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		BOOL const didMakeCurrent{ wglMakeCurrent(hdc, hglrc) };
		if (didMakeCurrent) {}
		else
		{
			DWORD error{ GetLastError() };
			PostQuitMessage(-1);
			return -1;
		}

		silnith::InitializeOpenGLState();

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

		assert(hglrc == wglGetCurrentContext());

		GLsizei const width{ LOWORD(lParam) };
		GLsizei const height{ HIWORD(lParam) };
		silnith::Resize(width, height);

		return 0;
	}
	case WM_TIMER:
	{
		assert(hglrc == wglGetCurrentContext());

		silnith::AdvanceAnimation();

		InvalidateRgn(hWnd, NULL, FALSE);
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

		silnith::DrawFrame();

		PAINTSTRUCT paintstruct;
		HDC const hdc{ BeginPaint(hWnd, &paintstruct) };
		if (hdc == NULL) {
			return -1;
		}
		//EnumDisplayMonitors(hdc, NULL, MonitorEnumProc, 0);

		//wglMakeCurrent(hdc, hglrc);

		//silnith::DrawFrame();

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

	return (int) msg.wParam;
}
