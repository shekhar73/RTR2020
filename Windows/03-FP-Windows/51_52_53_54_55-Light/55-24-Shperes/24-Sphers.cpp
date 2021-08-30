//Headers
#include <windows.h>
#include "24-Sphers.h"
#include <stdio.h>
#include <stdlib.h>
#include <gl/GL.h>
#include <gl/GLU.h>

// Library Include
#pragma comment(lib, "OPENGL32.LIB")
#pragma comment(lib, "GLU32.lib")

//Macro declarations
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
FILE* logFile = NULL;
SYSTEMTIME st;

// Global variables for Fullscreen
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
HWND ghwnd = NULL;

// Global variables for initialize and game loop
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;

// Global variables for Light

bool bLight = false;

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

// Axis Rotation
GLfloat anlgeForXRotataion = 0.0f;
GLfloat anlgeForYRotataion = 0.0f;
GLfloat anlgeForZRotataion = 0.0f;

// Quadric for  24 sphere
GLUquadric* quadric[24];
GLint keyPressed = 0;

GLfloat lightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis It is a Positional Light
//GLfloat lightSpecular[] = { 1.0f, 0.0f, 0.0f, 1.0f };

// WinMain
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

	// Creating a log file
	if (fopen_s(&logFile, "SomshekharLogFile.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Can not create desired file"),
			TEXT("ERROR"), MB_OK);
		exit(0);
	}

	// Centering of Window
	int X = GetSystemMetrics(SM_CXSCREEN);
	int Y = GetSystemMetrics(SM_CYSCREEN);

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
		TEXT("24 Shpere Application"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((X / 2) - (WIN_WIDTH / 2)),
		((Y / 2) - (WIN_HEIGHT / 2)),
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

	GetSystemTime(&st);

	//code
	switch (iMsg)
	{

	/*case WM_CREATE:
		MessageBox(hwnd, TEXT("Inside WM_CREATE"), TEXT("WM_CREATE"), MB_OK);
		break;*/

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return (0); //If break used then it will act as WM_PAINT else we have our own rendering function

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{

		case VK_ESCAPE:
		case ' ':
			DestroyWindow(hwnd);
			break;

		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		case 'L':
		case 'l':
			if (bLight == false)
			{
				glEnable(GL_LIGHTING);
				bLight = true;
			}
			else
			{
				glDisable(GL_LIGHTING);
				bLight = false;
			}
			break;

		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'x':
		case 'X':
			keyPressed = 1;
			anlgeForXRotataion = 0.0f;
			break;

		case 'y':
		case 'Y':
			keyPressed = 2;
			anlgeForYRotataion = 0.0f;
			break;

		case 'z':
		case 'Z':
			keyPressed = 3;
			anlgeForZRotataion = 0.0f;
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		fprintf(logFile, "Program executed successfully on : %d/%d/%d at TIME:%d:%d:%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
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

	// Depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//set clear color
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);

	/*glLightfv(GL_LIGHT0, GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightfv(GL_LIGHT0, GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_ambient);*/


	//////////////////////////////////////////////////////////////////
	// Initialization of light
	/////////////////////////////////////////////////////////////////
	//glLightfv(TypeOfLight, TypeOfProperty, Value);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++)
	{
		quadric[i] = gluNewQuadric();
	}

	//////////////////////////////////////////////////////////////////
	// Material Initialization
	//////////////////////////////////////////////////////////////////

	/*glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);*/

	Resize(WIN_WIDTH, WIN_HEIGHT); //Warmup call to resize
}

void Resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (width <= height) 
	{
		glOrtho(0.0f, //Left
			15.5f * ((GLfloat)(height) / (GLfloat)(width)),  // Right
			0.0f, // Bottom
			15.5f * ((GLfloat)(height) / (GLfloat)(width)), // Top
			-10.0f, // Near
			10.0f); // Far
	}
	else
	{
		glOrtho(-15.5f, //Left
			15.5f,  // Right
			-15.5f * ((GLfloat)(height) / (GLfloat)(width)), // Bottom
			15.5f * ((GLfloat)(height) / (GLfloat)(width)), // Top
			-100.0f, // Near
			100.0f); // Far
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void Display(void)
{
	// Local Function Declaration
	void draw24Sphere(void);

	if (keyPressed == 1)
	{
		glRotatef(anlgeForXRotataion, 1.0f, 0.0f, 0.0f);
		lightPosition[1] = anlgeForXRotataion;
	}
	else if (keyPressed == 2)
	{
		glRotatef(anlgeForYRotataion, 0.0f, 1.0f, 0.0f);
		lightPosition[2] = anlgeForYRotataion;
	}
	else if (keyPressed == 3)
	{
		glRotatef(anlgeForZRotataion, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = anlgeForZRotataion;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	
	draw24Sphere();

	SwapBuffers(ghdc);	
}

void draw24Sphere()
{

	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShineness;;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glTranslatef(6.0f, -3.0f, -30.0f);

	//////////////////////////////////////////////////////////
	// Emerald
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 1.0215f;
	materialAmbient[1] = 0.1745f;
	materialAmbient[2] = 0.0215f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 1.07568f;
	materialDiffuse[1] = 0.61424f;
	materialDiffuse[2] = 0.07568f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.633f;
	materialSpecular[1] = 0.727811f;
	materialSpecular[2] = 0.633f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.6f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 10.0f, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// Jade
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.135f;
	materialAmbient[1] = 0.2225f;
	materialAmbient[2] = 0.1575f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.54f;
	materialDiffuse[1] = 0.89f;
	materialDiffuse[2] = 0.63f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.316228f;
	materialSpecular[1] = 0.316228f;
	materialSpecular[2] = 0.316228f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.1f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-10.0f, 7.50f, 0.0f);

	gluSphere(quadric[1], 1.0f, 30, 30);



	//////////////////////////////////////////////////////////
	// obsidian
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.05375f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.06625f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.18275f;
	materialDiffuse[1] = 0.17f;
	materialDiffuse[2] = 0.22525f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.332741f;
	materialSpecular[1] = 0.328634f;
	materialSpecular[2] = 0.346435f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.3f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 5.0f, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// pearl
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.20725f;
	materialAmbient[2] = 0.20725f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 1.0f;
	materialDiffuse[1] = 0.829f;
	materialDiffuse[2] = 0.829f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.296648f;
	materialSpecular[1] = 0.296648f;
	materialSpecular[2] = 0.296648f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);


	materialShineness = 0.088f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 2.50f, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// ruby
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.1745f;
	materialAmbient[1] = 0.01175f;
	materialAmbient[2] = 0.01175f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.61424f					;
	materialDiffuse[1] = 0.04136f;
	materialDiffuse[2] = 0.04136f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.727811f;
	materialSpecular[1] = 0.626959f;
	materialSpecular[2] = 0.626959f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);


	materialShineness = 0.6f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 0.0f, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// turquoise
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.1f;
	materialAmbient[1] = 0.18725f;
	materialAmbient[2] = 0.1745f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.396f;
	materialDiffuse[1] = 0.74151f;
	materialDiffuse[2] = 0.69102f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.297254f;
	materialSpecular[1] = 0.30829f;
	materialSpecular[2] = 0.306678f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.1f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, -2.5f, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// brass
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.329412f;
	materialAmbient[1] = 0.223529f;
	materialAmbient[2] = 0.027451f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.780392;
	materialDiffuse[1] = 0.568627;
	materialDiffuse[2] = 0.113725;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.992157f;
	materialSpecular[1] = 0.941176f;
	materialSpecular[2] = 0.807843f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.21794872f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-7.5f, 10.0f, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// bronze 
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.2125f;
	materialAmbient[1] = 0.1275f;
	materialAmbient[2] = 0.054f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.714f;
	materialDiffuse[1] = 0.4284f;
	materialDiffuse[2] = 0.18144f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.393548f;
	materialSpecular[1] = 0.271906f;
	materialSpecular[2] = 0.166721f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.2f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-7.5f, 7.50f, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// chrome  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.25f;
	materialAmbient[2] = 0.25f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.4f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.774597f;
	materialSpecular[1] = 0.774597f;
	materialSpecular[2] = 0.774597f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.6f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-7.5, 5.0f, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// copper  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.19125f;
	materialAmbient[1] = 0.0735f;
	materialAmbient[2] = 0.0225f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.7038f;
	materialDiffuse[1] = 0.27048f;
	materialDiffuse[2] = 0.0828f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.256777f;
	materialSpecular[1] = 0.137622f;
	materialSpecular[2] = 0.086014f;
	materialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.1f  * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-7.5, 2.50f, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);



	//////////////////////////////////////////////////////////
	// gold  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.24725f;
	materialAmbient[1] = 0.1995f;
	materialAmbient[2] = 0.0745f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.75164f;
	materialDiffuse[1] = 0.60648f;
	materialDiffuse[2] = 0.22648f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.628281f;
	materialSpecular[1] = 0.555802f;
	materialSpecular[2] = 0.366065f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.4f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-7.5, 0.0f, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// silver  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.19225f;
	materialAmbient[1] = 0.19225f;
	materialAmbient[2] = 0.19225f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.50754f;
	materialDiffuse[1] = 0.50754f;
	materialDiffuse[2] = 0.50754f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.508273f;
	materialSpecular[1] = 0.508273f;
	materialSpecular[2] = 0.508273f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.4f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-7.5, -2.50f, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);


	// 3rd Coloumn
	//////////////////////////////////////////////////////////
	// black  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.01f;
	materialDiffuse[1] = 0.01f;
	materialDiffuse[2] = 0.01f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.50f;
	materialSpecular[1] = 0.50f;
	materialSpecular[2] = 0.50f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 10.0f, 0.0f);
	gluSphere(quadric[12], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// cyan  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.1f;
	materialAmbient[2] = 0.06f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.0f;
	materialDiffuse[1] = 0.50980392f;
	materialDiffuse[2] = 0.50980392f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.50196078f;
	materialSpecular[1] = 0.50196078f;
	materialSpecular[2] = 0.50196078f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-5.0, 7.50f, 0.0f);
	gluSphere(quadric[13], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// green  
	//////////////////////////////////////////////////////////
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.1f;
	materialDiffuse[1] = 0.35f;
	materialDiffuse[2] = 0.1f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.45f;
	materialSpecular[1] = 0.55f;
	materialSpecular[2] = 0.45f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 5.0f, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// red  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.0f;
	materialDiffuse[2] = 0.0f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.6f;
	materialSpecular[2] = 0.6f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 2.50f, 0.0f);
	gluSphere(quadric[15], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// white  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.55f;
	materialDiffuse[1] = 0.55f;
	materialDiffuse[2] = 0.55f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.70f;
	materialSpecular[1] = 0.70f;
	materialSpecular[2] = 0.70f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 0.0f, 0.0f);
	gluSphere(quadric[16], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// yellow plastic  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.60f;
	materialSpecular[1] = 0.60f;
	materialSpecular[2] = 0.50f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-5.0, -2.50f, 0.0f);
	gluSphere(quadric[17], 1.0, 30, 30);

	/// 4th coloumn
	//////////////////////////////////////////////////////////
	// black  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.02f;
	materialAmbient[1] = 0.02f;
	materialAmbient[2] = 0.02f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.01f;
	materialDiffuse[1] = 0.01f;
	materialDiffuse[2] = 0.01f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.4f;
	materialSpecular[1] = 0.4f;
	materialSpecular[2] = 0.4f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-2.50, 10.0f, 0.0f);
	gluSphere(quadric[18], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// cyan  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);



	materialSpecular[0] = 0.04f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.7f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);


	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-2.50, 7.50f, 0.0f);
	gluSphere(quadric[19], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// green  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4f; ;
	materialDiffuse[1] = 0.5f; ;
	materialDiffuse[2] = 0.4f; ;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);


	materialSpecular[0] = 0.04f;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-2.50, 5.0f, 0.0f);
	gluSphere(quadric[20], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// red  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.4f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.04f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-2.50, 2.50f, 0.0f);
	gluSphere(quadric[21], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// white  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.7f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-2.50, 0.0f, 0.0f);
	gluSphere(quadric[22], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// yellow rubber 
	//////////////////////////////////////////////////////////
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-2.50, -2.50f, 0.0f);
	gluSphere(quadric[23], 1.0, 30, 30);

	anlgeForXRotataion = anlgeForXRotataion + 1.1f;
	anlgeForYRotataion = anlgeForYRotataion + 1.1f;
	anlgeForZRotataion = anlgeForZRotataion + 1.1f;
	
	/*if (keyPressed == 1)
	{
		anlgeForXRotataion = anlgeForXRotataion + 0.1f;
	}
	else if (keyPressed == 2)
	{
		anlgeForYRotataion = anlgeForYRotataion + 0.1f;
	}
	else if (keyPressed == 3)
	{
		anlgeForZRotataion = anlgeForZRotataion + 0.1f;
	}*/
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

	for (int i = 0; i < 24; i++)
	{
		if (quadric[i])
		{
			gluDeleteQuadric(quadric[i]);
			quadric[i] = NULL;
		}
	}
	
	if (logFile)
	{
		fprintf(logFile, "Log file closed successfully and program is completed successfully");
		fclose(logFile);
		logFile = NULL;
	}
}

