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

#include "Color.h"
#include "Wing.h"
#include "CurveGenerator.h"

#include <cassert>
#include <deque>
#include <sstream>


typedef std::deque<silnith::Wing> wing_list;

size_t const numWings{ 40 };

UINT const updateDelayMilliseconds{ 35 };

GLuint glMajorVersion;
GLuint glMinorVersion;

GLuint wingDisplayList;
wing_list wings;

silnith::CurveGenerator radiusCurve{ 10.0f, -15.0f, 15.0f, false, 0.1f, 0.01f, 150 };
silnith::CurveGenerator angleCurve{ silnith::CurveGenerator::createGeneratorForAngles(0.0f, 2.0f, 0.05f, 120) };
silnith::CurveGenerator deltaAngleCurve{ silnith::CurveGenerator::createGeneratorForAngles(15.0f, 0.2f, 0.02f, 80) };
silnith::CurveGenerator deltaZCurve{ 0.5f, 0.4f, 0.7f, false, 0.01f, 0.001f, 200 };
silnith::CurveGenerator rollCurve{ silnith::CurveGenerator::createGeneratorForAngles(0.0f, 1.0f, 0.25f, 80) };
silnith::CurveGenerator pitchCurve{ silnith::CurveGenerator::createGeneratorForAngles(0.0f, 2.0f, 0.25f, 40) };
silnith::CurveGenerator yawCurve{ silnith::CurveGenerator::createGeneratorForAngles(0.0f, 1.5f, 0.25f, 50) };
silnith::CurveGenerator redCurve{ silnith::CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 95) };
silnith::CurveGenerator greenCurve{ silnith::CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 40) };
silnith::CurveGenerator blueCurve{ silnith::CurveGenerator::createGeneratorForColorComponents(0.0f, 0.04f, 0.01f, 70) };

UINT_PTR timer;

bool hasOpenGL(GLuint major, GLuint minor)
{
	return (glMajorVersion > major)
		|| (glMajorVersion == major && glMinorVersion >= minor);
}

void ParseOpenGLVersion(GLubyte const* glVersion)
{
	std::istringstream versionStringInput{ std::string{ reinterpret_cast<char const*>(glVersion) } };
	//std::basic_istringstream<GLubyte> versionStringInput{ std::basic_string<GLubyte>{glVersion} };

	versionStringInput >> glMajorVersion;
	GLubyte period;
	versionStringInput >> period;
	assert(period == '.');
	versionStringInput >> glMinorVersion;
}

/// <summary>
/// Selects and makes current an appropriate pixel format into the device context.
/// </summary>
/// <param name="hdc">a handle to the device context</param>
/// <returns>the identifier for the pixel format</returns>
int InitializeDeviceContext(HDC const& hdc)
{
	PIXELFORMATDESCRIPTOR pfd{
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

	int pixelformat{ ChoosePixelFormat(hdc, &pfd) };
	if (pixelformat == 0) {
		DWORD error{ GetLastError() };
		PostQuitMessage(-1);
		return -1;
	}

	//int foo = DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	BOOL pixelFormatSuccess{ SetPixelFormat(hdc, pixelformat, &pfd) };
	if (pixelFormatSuccess)
	{
	}
	else
	{
		DWORD error{ GetLastError() };
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
	HGLRC hglrc{ wglCreateContext(hdc) };
	BOOL success{ wglMakeCurrent(hdc, hglrc) };
	return hglrc;
}

void InitializeOpenGLState()
{
	GLubyte const* const glVendor{ glGetString(GL_VENDOR) };
	GLubyte const* const glRenderer{ glGetString(GL_RENDERER) };
	GLubyte const* const glVersion{ glGetString(GL_VERSION) };
	GLubyte const* const glExtensions{ glGetString(GL_EXTENSIONS) };

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

	glLoadIdentity();
	gluLookAt(0, 50, 50, 0, 0, 13, 0, 0, 1);
}

void InitializeWingDisplayList()
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

void InitializeWingList(void)
{
	GLuint const wingLists{ glGenLists(numWings) };
	for (GLuint displayList{ wingLists }; displayList < wingLists + numWings; displayList++) {
		// This initializes the list of wings to hold the allocated GL display lists.
		// These display list identifiers are reused throughout the lifetime of the program.
		wings.emplace_back(displayList);
		glNewList(displayList, GL_COMPILE);
		glEndList();
	}
}

void advanceAnimation(void)
{
	GLuint const displayList{ wings.back().getGLDisplayList() };
	wings.pop_back();
	silnith::Wing const& wing{ wings.emplace_front(displayList, radiusCurve.getNextValue(), angleCurve.getNextValue(),
		deltaAngleCurve.getNextValue(), deltaZCurve.getNextValue(),
		rollCurve.getNextValue(), pitchCurve.getNextValue(), yawCurve.getNextValue(),
		silnith::Color{ redCurve.getNextValue(), greenCurve.getNextValue(), blueCurve.getNextValue() },
		silnith::Color::WHITE) };

	// TODO: Do I need an HDC in order to execute these OpenGL commands?
	glNewList(displayList, GL_COMPILE);
	glPushMatrix();
	glRotatef(wing.getAngle(), 0, 0, 1);
	glTranslatef(wing.getRadius(), 0, 0);
	glRotatef(-(wing.getYaw()), 0, 0, 1);
	glRotatef(-(wing.getPitch()), 0, 1, 0);
	glRotatef(wing.getRoll(), 1, 0, 0);
	glCallList(wingDisplayList);
	glPopMatrix();
	glEndList();
}

void DrawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (hasOpenGL(1, 1))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPushMatrix();
		for (silnith::Wing const& wing : wings) {
			glTranslatef(0, 0, wing.getDeltaZ());
			glRotatef(wing.getDeltaAngle(), 0, 0, 1);

			silnith::Color const& edgeColor{ wing.getEdgeColor() };
			glColor3f(edgeColor.getRed(), edgeColor.getGreen(), edgeColor.getBlue());
			glCallList(wing.getGLDisplayList());
		}
		glPopMatrix();
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glPushMatrix();
	for (silnith::Wing const& wing : wings) {
		glTranslatef(0, 0, wing.getDeltaZ());
		glRotatef(wing.getDeltaAngle(), 0, 0, 1);

		silnith::Color const& color{ wing.getColor() };
		glColor3f(color.getRed(), color.getGreen(), color.getBlue());
		glCallList(wing.getGLDisplayList());
	}
	glPopMatrix();

	glFlush();
}

void Resize(GLsizei width, GLsizei height)
{
	GLdouble xmult{ 1.0 };
	GLdouble ymult{ 1.0 };
	if (width > height)
	{
		xmult = static_cast<GLdouble>(width) / static_cast<GLdouble>(height);
	}
	else
	{
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

// add to EXPORTS statement in module-definition (.def) file
LRESULT WINAPI ScreenSaverProcW(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// window created
		//LPCREATESTRUCTW createStruct{ (LPCREATESTRUCTW)lParam };
		//createStruct->cx;
		//createStruct->cy;
		// DirectDrawCreate()

		HDC hdc{ GetDC(hWnd) };

		InitializeDeviceContext(hdc);

		InitializeRenderingContext(hdc);

		InitializeOpenGLState();

		InitializeWingDisplayList();

		InitializeWingList();

		ReleaseDC(hWnd, hdc);

		timer = SetTimer(hWnd, 42, updateDelayMilliseconds, NULL);

		return 0;
	}
	case WM_DPICHANGED:
	{
		// GetSystemMetricsForDpi, AdjustWindowRectExForDpi, SystemParametersInfoForDpi, GetDpiForWindow
		WORD yAxisDPI{ HIWORD(wParam) };
		WORD xAxisDPI{ LOWORD(wParam) };
		LPRECT suggestedSizeAndPosition{ (RECT*)lParam };
		/*
		* TODO: Guard this call to Windows 8.1 and later.
		*/
		BOOL success{ SetWindowPos(hWnd, NULL, suggestedSizeAndPosition->left, suggestedSizeAndPosition->top, suggestedSizeAndPosition->right - suggestedSizeAndPosition->left, suggestedSizeAndPosition->bottom - suggestedSizeAndPosition->top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS) };
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

		HDC hdc{ GetDC(hWnd) };

		GLsizei width{ LOWORD(lParam) };
		GLsizei height{ HIWORD(lParam) };
		Resize(width, height);

		ReleaseDC(hWnd, hdc);

		return 0;
	}
	case WM_TIMER:
	{
		// TODO: Is it necessary to grab the HDC before issuing OpenGL rendering commands,
		// if those commands are merely compiling (but not executing) a new display list?
		HDC hdc{ GetDC(hWnd) };
		advanceAnimation();
		InvalidateRgn(hWnd, NULL, FALSE);
		ReleaseDC(hWnd, hdc);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT paintstruct;
		HDC hdc{ BeginPaint(hWnd, &paintstruct) };
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
		BOOL destroyed{ DestroyWindow(hWnd) };
		return 0;
	}
	case WM_DESTROY:
	{
		KillTimer(hWnd, timer);

		// window about to be destroyed
		HDC hdc{ GetDC(hWnd) };
		HGLRC hglrc{ wglGetCurrentContext() };
		if (hglrc != NULL)
		{
			wglMakeCurrent(hdc, NULL);
			wglDeleteContext(hglrc);
		}
		ReleaseDC(hWnd, hdc);

		//PostQuitMessage(0);
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