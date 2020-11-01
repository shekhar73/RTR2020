#include<windows.h>
#include"Animation_Cube_using_matrix.h"
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define _USE_MATH_DEFINES
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

GLfloat IdentityMatrix[16];
GLfloat TrnaslationMatrix[16];
GLfloat ScaleMatrix[16];
GLfloat RotationMatrix_x[16];
GLfloat RotationMatrix_y[16];
GLfloat RotationMatrix_z[16];

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

	IdentityMatrix[0] = 1.0f;
	IdentityMatrix[1] = 0.0f;
	IdentityMatrix[2] = 0.0f;
	IdentityMatrix[3] = 0.0f;
	IdentityMatrix[4] = 0.0f;
	IdentityMatrix[5] = 1.0f;
	IdentityMatrix[6] = 0.0f;
	IdentityMatrix[7] = 0.0f;
	IdentityMatrix[8] = 0.0f;
	IdentityMatrix[9] = 0.0f;
	IdentityMatrix[10] = 1.0f;
	IdentityMatrix[11] = 0.0f;
	IdentityMatrix[12] = 0.0f;
	IdentityMatrix[13] = 0.0f;
	IdentityMatrix[14] = 0.0f;
	IdentityMatrix[15] = 1.0f;

	TrnaslationMatrix[0] = 1.0f;
	TrnaslationMatrix[1] = 0.0f;
	TrnaslationMatrix[2] = 0.0f;
	TrnaslationMatrix[3] = 0.0f;
	TrnaslationMatrix[4] = 0.0f;
	TrnaslationMatrix[5] = 1.0f;
	TrnaslationMatrix[6] = 0.0f;
	TrnaslationMatrix[7] = 0.0f;
	TrnaslationMatrix[8] = 0.0f;
	TrnaslationMatrix[9] = 0.0f;
	TrnaslationMatrix[10] = 1.0f;
	TrnaslationMatrix[11] = 0.0f;
	TrnaslationMatrix[12] = 0.0f;
	TrnaslationMatrix[13] = 0.0f;
	TrnaslationMatrix[14] = -6.0f;
	TrnaslationMatrix[15] = 1.0f;

	ScaleMatrix[0] = 0.75f;
	ScaleMatrix[1] = 0.0f;
	ScaleMatrix[2] = 0.0f;
	ScaleMatrix[3] = 0.0f;
	ScaleMatrix[4] = 0.0f;
	ScaleMatrix[5] = 0.75f;
	ScaleMatrix[6] = 0.0f;
	ScaleMatrix[7] = 0.0f;
	ScaleMatrix[8] = 0.0f;
	ScaleMatrix[9] = 0.0f;
	ScaleMatrix[10] = 0.75f;
	ScaleMatrix[11] = 0.0f;
	ScaleMatrix[12] = 0.0f;
	ScaleMatrix[13] = 0.0f;
	ScaleMatrix[14] = 0.0f;
	ScaleMatrix[15] = 1.0f;

	//RotationMatrix_x[0] = 0.0f;

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

	static GLfloat angle_cube_rad = 0.0f;
	static GLfloat angle_cube = 1.0f;

	//Code
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);	

	/////////////////////////////////////////////////////////////////////////////////
	//Code for RECTANGLE
	/////////////////////////////////////////////////////////////////////////////////

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(IdentityMatrix);
	glMultMatrixf(TrnaslationMatrix);
	glMultMatrixf(ScaleMatrix);

	angle_cube_rad = t_angle * (M_PI / 180.0f);

	RotationMatrix_x[0] = 1.0f;
	RotationMatrix_x[1] = 0.0f;
	RotationMatrix_x[2] = 0.0f;
	RotationMatrix_x[3] = 0.0f;
	RotationMatrix_x[4] = 0.0f;
	RotationMatrix_x[5] = cos(angle_cube_rad);
	RotationMatrix_x[6] = sin(angle_cube_rad);
	RotationMatrix_x[7] = 0.0f;
	RotationMatrix_x[8] = 0.0f;
	RotationMatrix_x[9] = -sin(angle_cube_rad);
	RotationMatrix_x[10] = cos(angle_cube_rad);
	RotationMatrix_x[11] = 0.0f;
	RotationMatrix_x[12] = 0.0f;
	RotationMatrix_x[13] = 0.0f;
	RotationMatrix_x[14] = 0.0f;
	RotationMatrix_x[15] = 1.0f;

	RotationMatrix_y[0] = cos(angle_cube_rad);
	RotationMatrix_y[1] = 0.0f;
	RotationMatrix_y[2] = -sin(angle_cube_rad);
	RotationMatrix_y[3] = 0.0f;
	RotationMatrix_y[4] = 0.0f;
	RotationMatrix_y[5] = 1.0f;
	RotationMatrix_y[6] = 0.0f;
	RotationMatrix_y[7] = 0.0f;
	RotationMatrix_y[8] = sin(angle_cube_rad);
	RotationMatrix_y[9] = 0.0f;
	RotationMatrix_y[10] = cos(angle_cube_rad);
	RotationMatrix_y[11] = 0.0f;
	RotationMatrix_y[12] = 0.0f;
	RotationMatrix_y[13] = 0.0f;
	RotationMatrix_y[14] = 0.0f;
	RotationMatrix_y[15] = 1.0f;

	RotationMatrix_z[0] = cos(angle_cube_rad);
	RotationMatrix_z[1] = sin(angle_cube_rad);
	RotationMatrix_z[2] = 0.0f;
	RotationMatrix_z[3] = 0.0f;
	RotationMatrix_z[4] = -sin(angle_cube_rad);
	RotationMatrix_z[5] = cos(angle_cube_rad);
	RotationMatrix_z[6] = 0.0f;
	RotationMatrix_z[7] = 0.0f;
	RotationMatrix_z[8] = 0.0f;
	RotationMatrix_z[9] = 0.0f;
	RotationMatrix_z[10] = 1.0f;
	RotationMatrix_z[11] = 0.0f;
	RotationMatrix_z[12] = 0.0f;
	RotationMatrix_z[13] = 0.0f;
	RotationMatrix_z[14] = 0.0f;
	RotationMatrix_z[15] = 1.0f;

	glMultMatrixf(RotationMatrix_x);
	glMultMatrixf(RotationMatrix_y);
	glMultMatrixf(RotationMatrix_z);

	//glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -4.0f);
	//glScalef(0.75f, 0.75f, 0.75f);
	//glRotatef(r_angle, 1.0f, 0.0f, 0.0f);
	//glRotatef(r_angle, 0.0f, 1.0f, 0.0f);
	//glRotatef(r_angle, 0.0f, 0.0f, 1.0f);
	

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

	t_angle += 1.0f;
	if (t_angle >= 360.0f)
	{
		t_angle = 0.0f;
	}

	r_angle += 1.0f;
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

