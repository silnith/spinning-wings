/*
* TODO: Disable PCA in manifest.
* mark as DPI-aware
*/

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <deque>
#include <string>
#include <sstream>

#include <cassert>

#include "Color.h"
#include "Wing.h"
#include "CurveGenerator.h"

typedef std::deque<Wing> wing_list;

UINT const updateDelayMilliseconds = 35;

GLuint glMajorVersion;
GLuint glMinorVersion;

GLuint wingDisplayList;
wing_list wings;

CurveGenerator radiusChange{ 10.0f, -15.0f, 15.0f, false, 0.1f, 0.01f, 150 };
CurveGenerator angleChange{ CurveGenerator::createGeneratorForAngles(0, 2, 0.05f, 120) };
CurveGenerator deltaAngleChange{ CurveGenerator::createGeneratorForAngles(15, 0.2f, 0.02f, 80) };
CurveGenerator zDeltaChange{ 0.5f, 0.4f, 0.7f, false, 0.01f, 0.001f, 200 };
CurveGenerator rollChange{ CurveGenerator::createGeneratorForAngles(0, 1.0f, 0.25f, 80) };
CurveGenerator pitchChange{ CurveGenerator::createGeneratorForAngles(0, 2.0f, 0.25f, 40) };
CurveGenerator yawChange{ CurveGenerator::createGeneratorForAngles(0, 1.5f, 0.25f, 50) };
CurveGenerator redMovement{ CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 95) };
CurveGenerator greenMovement{ CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 40) };
CurveGenerator blueMovement{ CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 70) };

void advanceAnimation(void)
{
	wings.pop_back();
	wings.emplace_front(radiusChange.getNextValue(), angleChange.getNextValue(),
		deltaAngleChange.getNextValue(), zDeltaChange.getNextValue(),
		rollChange.getNextValue(), pitchChange.getNextValue(), yawChange.getNextValue(),
		Color{ redMovement.getNextValue(), greenMovement.getNextValue(), blueMovement.getNextValue() },
		Color::WHITE);
}

bool hasOpenGL(GLuint major, GLuint minor)
{
	return (glMajorVersion > major)
		|| (glMajorVersion == major && glMinorVersion >= minor);
}

void TimerProc(HWND hWnd, UINT foo, UINT_PTR bar, DWORD baz)
{
	advanceAnimation();
}

BOOL MonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT lpRect, LPARAM d)
{
	return TRUE;
}

/// <summary>
/// Selects and makes current an appropriate pixel format into the device context.
/// </summary>
/// <param name="hdc">a handle to the device context</param>
/// <returns>the identifier for the pixel format</returns>
int InitializeDeviceContext(HDC const & hdc)
{
	PIXELFORMATDESCRIPTOR pfd{};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cRedBits = 8;
	pfd.cRedShift = 0;
	pfd.cGreenBits = 8;
	pfd.cGreenShift = 0;
	pfd.cBlueBits = 8;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;
	pfd.cAlphaShift = 0;
	pfd.cAccumBits = 0;
	pfd.cAccumRedBits = 0;
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 0;
	pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.bReserved = 0;
	pfd.dwLayerMask = 0;
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;

	int pixelformat = ChoosePixelFormat(hdc, &pfd);
	if (pixelformat == 0) {
		DWORD error = GetLastError();
		PostQuitMessage(-1);
		return -1;
	}

	//int foo = DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	BOOL pixelFormatSuccess = SetPixelFormat(hdc, pixelformat, &pfd);
	if (pixelFormatSuccess)
	{
	}
	else
	{
		DWORD error = GetLastError();
		PostQuitMessage(-1);
		return -1;
	}

	return pixelformat;
}

/// <summary>
/// Initializes an OpenGL rendering context for an existing device context.
/// </summary>
/// <param name="hdc">a handle to the device context</param>
/// <returns>a handle to the rendering context</returns>
HGLRC InitializeRenderingContext(HDC const& hdc)
{
	HGLRC hglrc = wglCreateContext(hdc);
	BOOL success = wglMakeCurrent(hdc, hglrc);
	return hglrc;
}

void ParseOpenGLVersion(GLubyte const * glVersion)
{
	std::istringstream versionStringInput{ std::string{ reinterpret_cast<char const*>(glVersion) } };
	//std::basic_istringstream<GLubyte> versionStringInput{ std::basic_string<GLubyte>{glVersion} };

	versionStringInput >> glMajorVersion;
	GLubyte period;
	versionStringInput >> period;
	assert(period == '.');
	versionStringInput >> glMinorVersion;
}

void InitializeOpenGLState()
{
	GLubyte const* glVendor = glGetString(GL_VENDOR);
	GLubyte const* glRenderer = glGetString(GL_RENDERER);
	GLubyte const* glVersion = glGetString(GL_VERSION);
	GLubyte const* glExtensions = glGetString(GL_EXTENSIONS);

	assert(glVendor != NULL);
	assert(glRenderer != NULL);
	assert(glVersion != NULL);
	assert(glExtensions != NULL);

	ParseOpenGLVersion(glVersion);

	glEnable(GL_DEPTH_TEST);
	if (hasOpenGL(1, 1))
	{
		glPolygonOffset(-0.5, -2);
	}

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0);

	glEnable(GL_POLYGON_SMOOTH);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void InitializeDisplayList()
{
	wingDisplayList = glGenLists(1);
	glNewList(wingDisplayList, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glEnd();
	glEndList();
}

void DrawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 50, 50, 0, 0, 13, 0, 0, 1);

	if (hasOpenGL(1, 1))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPushMatrix();
		wing_list::size_type count = 0;
		for (Wing const & wing : wings) {
			glTranslatef(0, 0, wing.getZDelta());
			glPushMatrix();
			glRotatef((wing.getAngle()) + count * (wing.getDeltaAngle()), 0, 0, 1);
			glTranslatef(wing.getRadius(), 0, 0);
			glRotatef(-(wing.getYaw()), 0, 0, 1);
			glRotatef(-(wing.getPitch()), 0, 1, 0);
			glRotatef(wing.getRoll(), 1, 0, 0);
			glColor3f(wing.getEdgeColor().getRed(), wing.getEdgeColor().getGreen(), wing.getEdgeColor().getBlue());
			glCallList(wingDisplayList);
			glPopMatrix();

			count++;
		}
		glPopMatrix();
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	wing_list::size_type count = 0;
	for (Wing const & wing : wings) {
		glTranslatef(0, 0, wing.getZDelta());
		glPushMatrix();
		glRotatef((wing.getAngle()) + count * (wing.getDeltaAngle()), 0, 0, 1);
		glTranslatef(wing.getRadius(), 0, 0);
		glRotatef(-(wing.getYaw()), 0, 0, 1);
		glRotatef(-(wing.getPitch()), 0, 1, 0);
		glRotatef(wing.getRoll(), 1, 0, 0);
		glColor3f(wing.getColor().getRed(), wing.getColor().getGreen(), wing.getColor().getBlue());
		glCallList(wingDisplayList);
		glPopMatrix();

		count++;
	}

	glFlush();
}

void Resize(GLsizei width, GLsizei height)
{
	GLdouble xmult;
	GLdouble ymult;
	if (width > height)
	{
		xmult = static_cast<GLdouble>(width) / static_cast<GLdouble>(height);
		ymult = 1.0;
	}
	else
	{
		xmult = 1.0;
		ymult = static_cast<GLdouble>(height) / static_cast<GLdouble>(width);
	}

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(static_cast<GLdouble>(-20) * xmult, static_cast<GLdouble>(20) * xmult,
		static_cast<GLdouble>(-20) * ymult, static_cast<GLdouble>(20) * ymult,
		static_cast<GLdouble>(35), static_cast<GLdouble>(105));
	glMatrixMode(GL_MODELVIEW);
	// check GL errors
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		// window created
		LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;
		createStruct->cx;
		createStruct->cy;
		// DirectDrawCreate()

		HDC hdc = GetDC(hWnd);

		InitializeDeviceContext(hdc);

		InitializeRenderingContext(hdc);

		InitializeOpenGLState();

		InitializeDisplayList();

		ReleaseDC(hWnd, hdc);

		return 0;
	}
	case WM_DPICHANGED:
	{
		// GetSystemMetricsForDpi, AdjustWindowRectExForDpi, SystemParametersInfoForDpi, GetDpiForWindow
		WORD yAxisDPI = HIWORD(wParam);
		WORD xAxisDPI = LOWORD(wParam);
		LPRECT suggestedSizeAndPosition = (RECT*)lParam;
		/*
		* TODO: Guard this call to Windows 8.1 and later.
		*/
		BOOL success = SetWindowPos(hWnd, NULL, suggestedSizeAndPosition->left, suggestedSizeAndPosition->top, suggestedSizeAndPosition->right - suggestedSizeAndPosition->left, suggestedSizeAndPosition->bottom - suggestedSizeAndPosition->top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
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
			return DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}

		HDC hdc = GetDC(hWnd);

		GLsizei width = LOWORD(lParam);
		GLsizei height = HIWORD(lParam);
		Resize(width, height);

		ReleaseDC(hWnd, hdc);

		return 0;
	}
	case WM_TIMER:
	{
		//hdc = GetDC(hWnd);
		//hdc = GetDCEx(hWnd, NULL, 0);
		//int foo = ReleaseDC(hWnd, hdc);
		advanceAnimation();
		//InvalidateRect(hWnd, NULL, FALSE);
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
		HDC hdc = BeginPaint(hWnd, &paintstruct);
		if (hdc == NULL) {
			return -1;
		}
		//EnumDisplayMonitors(hdc, NULL, MonitorEnumProc, 0);

		DrawFrame();
		
		SwapBuffers(hdc);

		EndPaint(hWnd, &paintstruct);
		return 0;
	}
	case WM_CLOSE:
	{
		BOOL destroyed = DestroyWindow(hWnd);
		return 0;
	}
	case WM_DESTROY:
	{
		// window about to be destroyed
		HDC hdc = GetDC(hWnd);
		HGLRC hglrc = wglGetCurrentContext();
		if (hglrc != NULL)
		{
			wglMakeCurrent(hdc, NULL);
			wglDeleteContext(hglrc);
		}
		ReleaseDC(hWnd, hdc);
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	}

	return 0;
}

int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	wings.resize(40);

	// register the window class for the main window

	WNDCLASSEXW wndClassEx;
	wndClassEx.cbSize = sizeof(WNDCLASSEXW);
	wndClassEx.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = hInstance;
	wndClassEx.hIcon = NULL;  // LoadIcon
	wndClassEx.hCursor = NULL;  // LoadCursor
	// GetStockObject(BLACK_BRUSH);
	wndClassEx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.lpszClassName = L"Project1Class";
	wndClassEx.hIconSm = NULL;
	ATOM wndClassIdentifier = RegisterClassExW(&wndClassEx);
	if (wndClassIdentifier == 0)
	{
		return FALSE;
	}

	// create the main window

	DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_WINDOWEDGE;
	LPCWSTR className = (LPCWSTR) wndClassIdentifier;
	LPCWSTR windowName = L"Project1Window";
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int width = CW_USEDEFAULT;
	int height = 600;
	HWND windowParent = NULL;
	HMENU menu = NULL;
	HWND window = CreateWindowExW(extendedWindowStyle, className, windowName, windowStyle, x, y, width, height, windowParent, menu, hInstance, NULL);
	if (window == NULL) {
		// call GetLastError
		return FALSE;
	}

	// show the window

	ShowWindow(window, nShowCmd);
	UpdateWindow(window);

	SetTimer(window, 42, updateDelayMilliseconds, NULL);

	// start the message loop

	MSG msg;
	BOOL hasMessage = GetMessageW(&msg, NULL, 0, 0);
	while (hasMessage) {
		if (hasMessage == -1) {
			return -1;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		hasMessage = GetMessageW(&msg, NULL, 0, 0);
	}

	return (int) msg.wParam;
}
