#include<windows.h>
#include"3D.h"
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
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("3D Program"),
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
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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
	//Local Variable Declaration
	static GLfloat t_angle = 0.0f;
	static GLfloat r_angle = 0.0f;

	//Code
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.0f, 0.0f, -3.0f);

	glRotatef(t_angle, 0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);

		//////////////////////////////////////////////////////////////////////////////
		//Front View
		//////////////////////////////////////////////////////////////////////////////
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);	//Apex

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);	//Left Bottom

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.5f, -0.5f, 0.5f);	//Right Bottom

		//////////////////////////////////////////////////////////////////////////////
		//End of Front View
		//////////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////////
		//Right Side View
		//////////////////////////////////////////////////////////////////////////////
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.5f, -0.5f, 0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		//////////////////////////////////////////////////////////////////////////////
		//End of Right Side View
		//////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////
		//Back Side View
		//////////////////////////////////////////////////////////////////////////////
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		//////////////////////////////////////////////////////////////////////////////
		//End of Back Side View
		//////////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////////
		//Left Side View
		//////////////////////////////////////////////////////////////////////////////
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		//////////////////////////////////////////////////////////////////////////////
		//End of Left Side View
		//////////////////////////////////////////////////////////////////////////////

	glEnd();

	/////////////////////////////////////////////////////////////////////////////////
	//Code for RECTANGLE
	/////////////////////////////////////////////////////////////////////////////////

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.0f, 0.0f, -4.0f);

	glRotatef(r_angle, 1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

		//Front View
		glColor3f(0.5f, 0.0f, 0.0f);	//RED COLOR
		glVertex3f(0.5f,0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);

		//Right Side
		glColor3f(0.0f, 0.5f, 0.0f);	//GREEN COLOR
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);


		//Back Side
		glColor3f(0.0f, 0.6f, 0.6f);	//BLUE COLOR
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		//Left Side
		glColor3f(0.5f, 0.0f, 0.5f);	//MAGENTA COLOR
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

		//Top Side
		glColor3f(0.6f, 0.6f, 0.0f);	//YELLOW COLOR
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);

		//Bottom Sie
		glColor3f(0.5f, 0.5f, 0.5f);	//VIOLET COLOR
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);


	glEnd();

	t_angle += 0.1f;
	if (t_angle >= 360.0f)
	{
		t_angle = 0.0f;
	}

	r_angle += 0.1f;
	if (r_angle >= 360.0f)
	{
		r_angle = 0.0f;
	}
	
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

	if (logFile)
	{
		fprintf(logFile, "Log file closed successfully and program is completed successfully");
		fclose(logFile);
		logFile = NULL;
	}
}

