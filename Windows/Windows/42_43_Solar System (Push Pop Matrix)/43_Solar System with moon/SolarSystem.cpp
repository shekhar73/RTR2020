#include<windows.h>
#include"SolarSystem.h"
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

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

int day = 0;
int year = 0;
int earth = 0;

GLUquadric* quadric = NULL;


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
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Solar System"),
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

	case WM_CHAR:
	{
		switch (wParam)
		{
		case 0x44:						// D
			day = (day + 6) % 360;
			break;

		case 0x64:						// d
			day = (day - 6) % 360;
			break;

		case 0x45:
			earth = (earth + 3) % 360;	// E
			break;

		case 0x65:
			earth = (earth - 3) % 360;	// e
			break;

		case 0x59:						// Y
			year = (year + 3) % 360;
			break;

		case 0x79:						//y
			year = (year - 3) % 360;
			break;

		}
	}
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
	pfd.cDepthBits = 32;
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
	glClear(GL_COLOR_BUFFER_BIT);

	// Model View Stack
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0.0f, 0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glPushMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 0.0f);


	//////////////////////////////////////////////////////////////////////
	// Draw SUN
	//////////////////////////////////////////////////////////////////////
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.75f, 30, 30);

	glPopMatrix();

	glPushMatrix();

	//Rotate earth around SUN
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);

	// Move Earth by 2 position from SUN
	glTranslatef(1.5f, 0.0f, 0.0f);

	// Rotate earths N-S pole from X-axis to Y-axis
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	// Rotate earth to itself (i.e. Y-axis)
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Wired frame
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0.4f, 0.9f, 1.0f);

	// Draw Earth
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f, 20, 20);


	// Moon code

	// Rotate moon around earth
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)earth, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.50f, 0.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);

	quadric = gluNewQuadric();
	gluSphere(quadric, 0.1f, 20, 20);

	glPopMatrix();
	SwapBuffers(ghdc);
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

	if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}

	if (logFile)
	{
		fprintf(logFile, "Log file closed successfully and program is completed successfully");
		fclose(logFile);
		logFile = NULL;
	}
}
