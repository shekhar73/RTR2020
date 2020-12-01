//Headers
#include <windows.h>
#include "LightAndMaterial.h"
#include <stdio.h>
#include <gl/GL.h>
#include <gl/GLU.h>

//Macro declarations
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib, "OPENGL32.LIB")
#pragma comment(lib, "GLU32.lib")

//Global variable declarations
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE* logFile = NULL;
bool gbActiveWindow = false;

bool bLight = false;

//GLfloat lightAmbient[]		= { 0.0f, 0.0f, 0.f, 1.0f };			// Gray light
//GLfloat lightDiffuse[]		= { 1.0f, 1.0f, 1.0f, 1.0f };			// White light
//GLfloat lightPosition[]		= { 100.0f, 100.0f, 100.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis
//GLfloat lightSpecular[]		= { 1.0f, 1.0f, 1.0f, 1.0f };			// White Light
//
//GLfloat materialAmbient[]	= { 0.0f, 0.0f, 0.f, 1.0f };
//GLfloat materialDiffuse[]	= { 1.0f, 1.0f, 1.0f, 1.0f };			// White light
//GLfloat materialSpecular[]	= { 1.0f, 1.0f, 1.0f, 1.0f };			// White Light
//
//GLfloat materialShineness = 50.0f;


GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };			// Gray light
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// White light
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// White Light

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };			// White light
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };		// White Light

GLfloat materialShineness = 128.0f;

GLUquadric* quadric = NULL;

//Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Light And Material Application");

	bool bDone = false;

	//Local function declarations
	void Initialize(void);
	void Display(void);

	if (fopen_s(&logFile, "SomshekharLogFile.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Can not create desired file"),
			TEXT("ERROR"), MB_OK);
		exit(0);
	}

	//code
	//Initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, (MAKEINTRESOURCE(MYICON)));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, (MAKEINTRESOURCE(MYICON)));

	//Register Above Class
	RegisterClassEx(&wndclass);


	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Light And Material Application"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2)),
		((GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2)),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	Initialize();
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Game loop
	while (false == bDone)
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
			if (true == gbActiveWindow)
			{
				//Here you should call update function for OpenGL rendering

				//Here you should Display function for OpenGL rendering
				Display();
			}
		}
	}

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Local function declarations
	void ToggleFullScreen(void);
	void Resize(int, int);
	void Uninitialize(void);


	//code
	switch (iMsg)
	{

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_CREATE:
		fprintf(logFile, "Inside WM_CREATE=>India is my country and I love my country. !!!\n");
		break;

	case WM_ERASEBKGND:
		return (0); //If break used then it will act as WM_PAINT else we have our own rendering function

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

		case 'L':
		case 'l':
			if (bLight == true)
			{
				glEnable(GL_LIGHTING);
				bLight = false;
			}
			else
			{
				glDisable(GL_LIGHTING);
				bLight = true;
			}
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
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	//Local Variable Declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };


	//Code
	if (false == gbFullscreen)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW)); //Removed WS_OVERLAPPEDWINDOW
				SetWindowPos(ghwnd, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);

			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
		ShowCursor(TRUE);
		gbFullscreen = false;

	}

}

void Initialize(void)
{
	//Function declaration
	void Resize(int, int);

	//Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//Code
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
	pfd.cDepthBits = 32;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (0 == iPixelFormatIndex)
	{
		fprintf(logFile, "ChoosePixelFormat() failed\n");
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(logFile, "SetPixelFormat() failed\n");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);

	if (NULL == ghrc)
	{
		fprintf(logFile, "wglCreateConext() failed\n");
		DestroyWindow(ghwnd);
	}

	if (FALSE == wglMakeCurrent(ghdc, ghrc))
	{
		fprintf(logFile, "wglMakeCurrent() failed\n");
		DestroyWindow(ghwnd);
	}

	//set clear color
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glEnable(GL_LIGHT1);

	Resize(WIN_WIDTH, WIN_HEIGHT); //Warmup call to resize
}

void Resize(int width, int height)
{
	//code
	if (0 == height)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f
	);
}

void Display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.55f, 0.0f, -3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f, 30, 30);	// Normals are calculated by gluSpere it self, we dont need to write Normal function explicitly.

	SwapBuffers(ghdc);
}

void Uninitialize()
{
	//Code
	if (true == gbFullscreen)
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

