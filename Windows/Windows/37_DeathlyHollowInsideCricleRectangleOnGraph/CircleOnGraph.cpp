#include<windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.14

#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "GLU32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE* logFile = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszcmdLine, int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("My App");

	void Display(void);
	void Initialize(void);
	bool bDone = false;

	if (fopen_s(&logFile, "SOMSHEKHAR.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Can not create desired file"), TEXT("ERROR"), MB_OK);
		exit(0);
	}

	wndclass.cbSize = sizeof(wndclass);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Circle On Graph"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2)),
		((GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2)),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghwnd = hwnd;

	Initialize();
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				Display();
			}
		}
	}

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen(void);
	void Resize(int, int);
	void Uninitialize(void);
	void Display(void);

	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_CREATE:
		fprintf(logFile, "Inside WM_CREATE\n");
		break;

	case WM_ERASEBKGND:
		return (0);

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		Uninitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	MONITORINFO mi = { sizeof(MONITORINFO) };

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos(ghwnd, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void Initialize(void)
{
	void Resize(int, int);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ghdc = GetDC(ghwnd);

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		fprintf(logFile, "ChoosePixelFomat() failed\n");
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(logFile, "SetPixelFormat() falied\n");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		fprintf(logFile, "wglCreateConext() failed\n");
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(logFile, "wglMakeCurrent() failed\n");
		DestroyWindow(ghwnd);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{
	GLfloat i = 0.0f, j = 0.0f;
	// Local Functions Declarations
	GLfloat getSidesOfTriangle(GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat getCenter(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat getRadius(GLfloat, GLfloat, GLfloat);
	GLfloat getCenterOfLine(GLfloat, GLfloat);


	// Local Variable Declarations
	GLfloat a, b, c = 0.0f;
	GLfloat inCenterX = 0.0f, inCenterY = 0.0f;
	GLfloat inRadius = 0.0f;
	GLfloat x1_base = 0.0f, y1_base = 0.0f;

	GLfloat angle = 0.0f, axis;
	GLint CirclePoints = 1000;

	GLfloat x1 = 0.0f, y1 = 0.5f;
	GLfloat x2 = -0.5f, y2 = -0.5f;
	GLfloat x3 = 0.5f, y3 = -0.5f;

	static GLfloat x_point = x2, y_point = y2;
	static GLfloat x_point_circle = 1.50f, y_point_circle = 0.75f;
	static GLfloat x_point_triangle = 1.50f, y_point_triangle = 0.75f;
	static GLfloat angle_rotate = 0.0f;

	a = getSidesOfTriangle(x2, y2, x3, y3);
	b = getSidesOfTriangle(x3, y3, x1, y1);
	c = getSidesOfTriangle(x1, y1, x2, y2);

	x1_base = getCenterOfLine(x3, y3);
	y1_base = getCenterOfLine(x2, y2);

	inCenterX = getCenter(a, b, c, x1, x2, x3);
	inCenterY = getCenter(a, b, c, y1, y2, y3);
	inRadius = getRadius(a, b, c);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	//////////////////////////////////////////////////////////////////////////
	//X-axis
	/////////////////////////////////////////////////////////////////////////

	//Red Color
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	// Single X-axis
	glVertex3f(-1.5f, 0.0f, 0.0f);
	glVertex3f(1.5f, 0.0f, 0.0f);

	glEnd();

	
	///////////////////////////////////////////////////////////////////////////////
	// Vertical Y-axis graph Start
	///////////////////////////////////////////////////////////////////////////////

	//Red Color
	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);

	// Single Y-axis
	glVertex3f(0.0f, -1.5f, 0.0f);
	glVertex3f(0.0f, 1.5f, 0.0f);

	glEnd();

	///////////////////////////////////////////////////////////////////////////

	glBegin(GL_LINES);

	//Blue Color
	glColor3f(0.0f, 0.0f, 1.0f);

	//Horizontal 20 Lines above X-axis
	for (i = -1.5f; i <= 1.5f; i = i + 0.035f)
	{
		glVertex3f(-1.5f, i, 0.0f);
		glVertex3f(1.5f, i, 0.0f);
	}

	glEnd();


	////////////////////////////////////////////////////////////////////////////////
	// End of Horizontal (X-axis) Graph
	////////////////////////////////////////////////////////////////////////////////


	
	//////////////////////////////////////////////////////////////////////////////////
	//Y-Axis graph
	//////////////////////////////////////////////////////////////////////////////////
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);

	for (i = -1.5f; i <= 1.5f; i = i + 0.035f)
	{
		glVertex3f(i, -1.5f, 0.0f);
		glVertex3f(i, 1.5f, 0.0f);
	}

	glEnd();

	///////////////////////////////////////////////////////////////////////////////////
	// End of Vertical Graph
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	// Triangle
	///////////////////////////////////////////////////////////////////////////////////

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (x_point_triangle < 0.0f)
	{
		glTranslatef(0.0f, 0.0f, -3.0f);
	}
	else
	{
		glTranslatef(-x_point_triangle, -y_point_triangle, -3.0f);
	}
	//glRotatef(angle_rotate, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);

	glColor3f(1.0f, 1.0f, 0.0f);

	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y2, 0.0f);

	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x3, y3, 0.0f);

	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x1, y1, 0.0f);

	glEnd();

	///////////////////////////////////////////////////////////////////////////////////
	// End of Triangle
	///////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////
	// Inside Circle
	///////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (y_point_circle < 0.0f)
	{
		glTranslatef(inCenterX, inCenterY, -3.0f);

	}
	else
	{
		glTranslatef(x_point_circle, -y_point_circle, -3.0f);
	}

	//glRotatef(angle_rotate, 0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);

	glColor3f(0.0f, 1.0f, 0.0f);

	for (int i = 0; i <= CirclePoints; i++)
	{
		angle = 2 * PI * i / CirclePoints;
		glVertex3f(inRadius * cos(angle), inRadius * sin(angle), 0.0f);
	}

	glEnd();

	///////////////////////////////////////////////////////////////////////////////////
	// End of Circle
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	// Line
	///////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	/*if (y_point > 0.0f)
	{
		glTranslatef(x_point, y_point, -3.0f);
	}
	else
	{
		glTranslatef(x2, y2, -3.0f);
	}*/
	glTranslatef(0.0f, 0.0f, -3.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);

	//Left Vertical Line
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);

	//Right Vertical Line
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);

	//Top Horizontal Line
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);

	glEnd();
	///////////////////////////////////////////////////////////////////////////////////
	// End of Line
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	// Outer Circle
	///////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);

	glBegin(GL_POINTS);

	glColor3f(0.0f, 1.0f, 0.0f);

	for (int i = 0; i <= CirclePoints; i++)
	{
		angle = 2 * PI * i / CirclePoints;
		glVertex3f(cos(angle) / 1.4f, sin(angle) / 1.4f, 0.0f);
	}

	glEnd();

	///////////////////////////////////////////////////////////////////////////////////
	// End of Circle
	///////////////////////////////////////////////////////////////////////////////////

	x_point_circle -= 0.00030f;
	y_point_circle -= 0.000150f;
	x_point_triangle -= 0.00030f;
	y_point_triangle -= 0.000150f;
	y_point -= 0.00020f;

	angle_rotate += 0.1f;
	if (angle_rotate >= 360.0f)
	{
		angle_rotate = 0.0f;
	}
	
	SwapBuffers(ghdc);
}



GLfloat DistanceBetweenTwoPoints(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	GLfloat a = x2 - x1;
	GLfloat b = y2 - y1;
	GLfloat distance = sqrt(pow(a, 2) + pow(b, 2));
	distance = distance * 1.0;
	return distance;
}

GLfloat getSidesOfTriangle(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By)
{
	GLfloat DistanceBetweenTwoPoints(GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat distance = 0.0f;
	distance = DistanceBetweenTwoPoints(Ax, Ay, Bx, By);
	return distance;
}

GLfloat getCenter(GLfloat a, GLfloat b, GLfloat c, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat ans = 0.0f;
	ans = ((a * x) + (b * y) + (c * z)) / (a + b + c);
	return ans;
}

// Radius of circle
GLfloat getRadius(GLfloat a, GLfloat b, GLfloat c)
{
	GLfloat s = 0.0f, area = 0.0f;
	s = (a + b + c) / 2.0f;    // Semi-perimeter 
	area = sqrt(s * (s - a) * (s - b) * (s - c));
	return (GLfloat)(area / s);
}


// Midpoint of line
GLfloat getCenterOfLine(GLfloat Ax, GLfloat Bx)
{
	GLfloat ans = (Ax + Bx) / 2.0f;
	return ans;
}

void Uninitialize()
{
	//Code
	if (true == gbFullScreen)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (logFile)
	{
		fprintf(logFile, "Log file closed successfully and program is completed successfully");
		fclose(logFile);
		logFile = NULL;
	}
}

