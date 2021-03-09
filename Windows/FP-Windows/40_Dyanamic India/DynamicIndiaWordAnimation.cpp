#include<windows.h>
#include"DynamicIndiaWordAnimation.h"
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.14f
//#define MYTIMER 200

#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "Winmm")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE* logFile = NULL;
static int iCount = 0;

void circle(void);

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
		TEXT("Dyanamic India"),
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
	/*static HBITMAP hBitmap = NULL;
	PAINTSTRUCT ps;
	RECT rc;*/

	void ToggleFullScreen(void);
	void Resize(int, int);
	void Uninitialize(void);
	void Display(void);

	switch (iMsg)
	{
	case WM_DESTROY:
		Uninitialize();
		PostQuitMessage(0);
		break;
	case WM_CREATE:

		PlaySound(TEXT("song.wav"), NULL, SND_ASYNC);

		/*hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(MYBITMAP));
		if (hBitmap == NULL) {
			MessageBox(hwnd, TEXT("Could Not Load Bitmap!!"), TEXT("Error!"), MB_OK);
		}*/
		fprintf(logFile, "Inside WM_CREATE\n");
		break;



	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
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

		/*case WM_PAINT:
			BITMAP bitmap;
			HDC hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rc);
			HDC memoryHdc = CreateCompatibleDC(NULL);
			SelectObject(memoryHdc, hBitmap);
			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			StretchBlt(hdc,
				200,
				80,
				1200,
				600,
				memoryHdc,
				0,
				0,
				bitmap.bmWidth,
				bitmap.bmHeight,
				SRCCOPY);

			DeleteDC(memoryHdc);
			EndPaint(hwnd, &ps);
			break;*/

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

	GLfloat i = 1.0f, j = 1.0f;
	static int k = 0;
	GLfloat angle = 0.0f, GLj = 1.0f;
	const int circlePoints = 333;
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	static GLfloat z = -9.0f;

	static GLfloat t_angle = 0.0f;

	void IAFPlane(void);


	glClear(GL_COLOR_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////////
	//Circle
	//////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, z);


	glRotatef(t_angle, x, y, z);

	glBegin(GL_LINES);

	k = 0;
	for (GLj = 1.0f; GLj <= 30.0f; GLj += 0.05f)
	{
		k++;
		if (k == 1)
		{
			glColor3f(1.0f, 0.5f, 0.0f);
		}
		if (k == 2)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		if (k == 3)
		{
			glColor3f(0.0, 1.0, 0.0); //Green
			k = 0;
		}

		for (int i = 0; i < circlePoints; i++)
		{
			angle = 2 * PI * i / circlePoints;
			glVertex3f(cos(angle) / GLj, sin(angle) / GLj, 0.0f);
		}

	}


	glEnd();
	z = z + 0.01f;
	if (z >= 9.0f)
	{
		z = -9.0f;
	}
	t_angle += 0.05f;
	if (t_angle >= 360.0f)
	{
		t_angle = 0.0f;
	}

	static GLfloat AxisChange = 0.0018f;
	void callIAF();
	void flag();



	////////////////////////////////////////////////////////////////////////////////////////////
	// I
	////////////////////////////////////////////////////////////////////////////////////////////

	static GLfloat xAxisForI = -2.5f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (xAxisForI <= -1.1f)
	{
		glTranslatef(xAxisForI, 0.0f, -3.0f);
	}
	else
	{
		glTranslatef(-1.1f, 0.0f, -3.0f);
	}


	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner	

	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner




	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner


	glEnd();


	if (xAxisForI <= -1.1f)
	{
		xAxisForI += AxisChange;

	}
	else
	{
		xAxisForI = -1.1f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of I
	////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////
	// N
	////////////////////////////////////////////////////////////////////////////////////////////

	static GLfloat yAxisForN = 1.5f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (yAxisForN >= 0.0f)
	{
		glTranslatef(-0.6f, yAxisForN, -3.0f);
	}
	else
	{
		glTranslatef(-0.6f, 0.0f, -3.0f);
	}



	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(0.3f, 0.3f, 0.0f);
	glVertex3f(0.2f, 0.3f, 0.0f);

	glVertex3f(0.2f, 0.3f, 0.0f);
	glVertex3f(0.2f, 0.1f, 0.0f);

	glVertex3f(0.2f, 0.1f, 0.0f);
	glVertex3f(0.3f, 0.1f, 0.0f);

	glVertex3f(0.3f, 0.1f, 0.0f);
	glVertex3f(0.3f, 0.3f, 0.0f);



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(0.3f, 0.1f, 0.0f);
	glVertex3f(0.2f, 0.1f, 0.0f);

	glVertex3f(0.2f, 0.1f, 0.0f);
	glVertex3f(0.2f, -0.1f, 0.0f);

	glVertex3f(0.2f, -0.1f, 0.0f);
	glVertex3f(0.3f, -0.1f, 0.0f);

	glVertex3f(0.3f, -0.1f, 0.0f);
	glVertex3f(0.3f, 0.1f, 0.0f);


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.3f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.1f, 0.0f);

	glVertex3f(0.2f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.3f, 0.0f);

	glVertex3f(0.2f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.3f, 0.0f);

	glVertex3f(0.3f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.1f, 0.0f);





	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);


	glVertex3f(-0.3f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);

	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.2f, 0.1f, 0.0f);

	glVertex3f(-0.2f, 0.1f, 0.0f);
	glVertex3f(-0.2f, 0.3f, 0.0f);



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);

	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(-0.2f, -0.1f, 0.0f);

	glVertex3f(-0.2f, -0.1f, 0.0f);
	glVertex3f(-0.2f, 0.1f, 0.0f);

	glVertex3f(-0.2f, 0.1f, 0.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(-0.3f, -0.3f, 0.0f);

	glVertex3f(-0.3f, -0.3f, 0.0f);
	glVertex3f(-0.2f, -0.3f, 0.0f);

	glVertex3f(-0.2f, -0.3f, 0.0f);
	glVertex3f(-0.2f, -0.1f, 0.0f);

	glVertex3f(-0.2f, -0.1f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);



	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);

	glVertex3f(-0.3f, 0.3f, 0.0f);
	glVertex3f(-0.13f, 0.1f, 0.0f);

	glVertex3f(-0.13f, 0.1f, 0.0f);
	glVertex3f(-0.03f, 0.1f, 0.0f);

	glVertex3f(-0.03f, 0.1f, 0.0f);
	glVertex3f(-0.2f, 0.3f, 0.0f);


	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.13f, 0.1f, 0.0f);
	glVertex3f(0.03f, -0.1f, 0.0f);

	glVertex3f(0.03f, -0.1f, 0.0f);
	glVertex3f(0.13f, -0.1f, 0.0f);

	glVertex3f(0.13f, -0.1f, 0.0f);
	glVertex3f(-0.03f, 0.1f, 0.0f);

	glVertex3f(-0.03f, 0.1f, 0.0f);
	glVertex3f(-0.13f, 0.1f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR
	glVertex3f(0.03f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.3f, 0.0f);

	glVertex3f(0.2f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.3f, 0.0f);

	glVertex3f(0.3f, -0.3f, 0.0f);
	glVertex3f(0.13f, -0.1f, 0.0f);

	glVertex3f(0.13f, -0.1f, 0.0f);
	glVertex3f(0.03f, -0.1f, 0.0f);

	glEnd();

	if (yAxisForN >= 0.0f)
	{
		yAxisForN -= AxisChange;

	}
	else
	{
		xAxisForI = 0.0f;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of N
	////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// D
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static GLfloat zAxisForD = -50.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (zAxisForD <= -3.0f)
	{
		glTranslatef(0.05f, 0.0f, zAxisForD);
	}
	else
	{
		glTranslatef(0.05f, 0.0f, -3.0f);
	}




	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(-0.1f, 0.3f, 0.0f);	//1
	glVertex3f(-0.2f, 0.3f, 0.0f);	//2

	glVertex3f(-0.2f, 0.3f, 0.0f);	//2
	glVertex3f(-0.2f, 0.1f, 0.0f);	//3

	glVertex3f(-0.2f, 0.1f, 0.0f);	//3
	glVertex3f(-0.1f, 0.1f, 0.0f);	//4

	glVertex3f(-0.1f, 0.1f, 0.0f);	//4
	glVertex3f(-0.1f, 0.3f, 0.0f);	//1


	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.1f, 0.1f, 0.0f);	//4
	glVertex3f(-0.2f, 0.1f, 0.0f);	//3

	glVertex3f(-0.2f, 0.1f, 0.0f);	//3
	glVertex3f(-0.2f, -0.1f, 0.0f);	//6

	glVertex3f(-0.2f, -0.1f, 0.0f);	//6
	glVertex3f(-0.1f, -0.1f, 0.0f);	//5

	glVertex3f(-0.1f, -0.1f, 0.0f);	//5
	glVertex3f(-0.1f, 0.1f, 0.0f);	//4


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(-0.1f, -0.1f, 0.0f);	//5
	glVertex3f(-0.2f, -0.1f, 0.0f);	//6

	glVertex3f(-0.2f, -0.1f, 0.0f);	//6
	glVertex3f(-0.2f, -0.3f, 0.0f);	//7

	glVertex3f(-0.2f, -0.3f, 0.0f);	//7
	glVertex3f(-0.1f, -0.3f, 0.0f);	//8

	glVertex3f(-0.1f, -0.3f, 0.0f);	//8
	glVertex3f(-0.1f, -0.1f, 0.0f);	//5


	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(-0.1f, 0.2f, 0.0);	//9
	glVertex3f(-0.1f, 0.3f, 0.0f);	//1

	glVertex3f(-0.1f, 0.3f, 0.0f);	//1
	glVertex3f(0.1f, 0.3f, 0.0);	//12

	glVertex3f(0.1f, 0.3f, 0.0);	//12
	glVertex3f(0.2f, 0.2f, 0.0);	//11

	glVertex3f(0.2f, 0.2f, 0.0);	//11
	glVertex3f(-0.1f, 0.2f, 0.0);	//9



	glVertex3f(0.05f, 0.2f, 0.0);	//10
	glVertex3f(0.2f, 0.2f, 0.0);	//11


	glVertex3f(0.2f, 0.2f, 0.0);	//11
	glVertex3f(0.2f, 0.15f, 0.0);	//24


	glVertex3f(0.2f, 0.15f, 0.0);	//24
	glVertex3f(0.1f, 0.15f, 0.0);	//23

	glVertex3f(0.1f, 0.15f, 0.0);	//23
	glVertex3f(0.05f, 0.2f, 0.0);	//10



	glVertex3f(0.2f, 0.15f, 0.0);	//24
	glVertex3f(0.1f, 0.15f, 0.0);	//23

	glVertex3f(0.1f, 0.15f, 0.0);	//23
	glVertex3f(0.1f, 0.1f, 0.0);	//14

	glVertex3f(0.1f, 0.1f, 0.0);	//14
	glVertex3f(0.2f, 0.1f, 0.0);	//13

	glVertex3f(0.2f, 0.1f, 0.0);	//13
	glVertex3f(0.2f, 0.15f, 0.0);	//24


	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR
	glVertex3f(0.2f, 0.1f, 0.0);	//13
	glVertex3f(0.1f, 0.1f, 0.0);	//14

	glVertex3f(0.1f, 0.1f, 0.0);	//14
	glVertex3f(0.1f, -0.1f, 0.0f);	//15

	glVertex3f(0.1f, -0.1f, 0.0f);	//15
	glVertex3f(0.2f, -0.1f, 0.0f);	//16

	glVertex3f(0.2f, -0.1f, 0.0f);	//16
	glVertex3f(0.2f, 0.1f, 0.0);	//13



	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.1f, -0.1f, 0.0f);	//15
	glVertex3f(0.2f, -0.1f, 0.0f);	//16

	glVertex3f(0.2f, -0.1f, 0.0f);	//16
	glVertex3f(0.2f, -0.15f, 0.0f);		//21

	glVertex3f(0.2f, -0.15f, 0.0f);		//21
	glVertex3f(0.1f, -0.15f, 0.0f);		//22

	glVertex3f(0.1f, -0.15f, 0.0f);		//22
	glVertex3f(0.1f, -0.1f, 0.0f);	//15



	glVertex3f(0.2f, -0.15f, 0.0f);		//21
	glVertex3f(0.1f, -0.15f, 0.0f);		//22

	glVertex3f(0.1f, -0.15f, 0.0f);		//22
	glVertex3f(0.05f, -0.2f, 0.0f);	//18

	glVertex3f(0.05f, -0.2f, 0.0f);	//18
	glVertex3f(0.2f, -0.2f, 0.0f);	//19

	glVertex3f(0.2f, -0.2f, 0.0f);	//19
	glVertex3f(0.2f, -0.15f, 0.0f);		//21



	glVertex3f(0.2f, -0.2f, 0.0f);		//19
	glVertex3f(-0.1f, -0.2f, 0.0f);		//17

	glVertex3f(-0.1f, -0.2f, 0.0f);		//17
	glVertex3f(-0.1f, -0.3f, 0.0f);		//8

	glVertex3f(-0.1f, -0.3f, 0.0f);		//8
	glVertex3f(0.1f, -0.3f, 0.0f);		//20

	glVertex3f(0.1f, -0.3f, 0.0f);		//20
	glVertex3f(0.2f, -0.2f, 0.0f);		//19


	glEnd();

	if (zAxisForD <= -3.0f)
	{
		zAxisForD += 0.065f;

	}
	else
	{
		zAxisForD = -3.0f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of D
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////
	// I
	////////////////////////////////////////////////////////////////////////////////////////////

	
	static GLfloat NegYAxisForI = -1.5f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	if (NegYAxisForI <= 0.0f)
	{
		glTranslatef(0.45f, NegYAxisForI, -3.0f);
	}
	else
	{
		glTranslatef(0.45f, 0.0f, -3.0f);
	}



	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner	

	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner




	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner


	glEnd();


	if (NegYAxisForI <= 0.0f)
	{
		NegYAxisForI += AxisChange;

	}
	else
	{
		NegYAxisForI = 0.0f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of I
	////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// A
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	static GLfloat NegXAxisForA = 2.5f;
	static GLfloat axis = -10.0f;



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (NegXAxisForA >= 0.9f)
	{
		glTranslatef(NegXAxisForA, 0.0f, -3.0f);
	}
	else
	{
		glTranslatef(0.9f, 0.0f, -3.0f);
	}

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(0.06f, 0.3f, 0.0f);	//1
	glVertex3f(-0.06f, 0.3f, 0.0f);	//2

	glVertex3f(-0.06f, 0.3f, 0.0f);	//2
	glVertex3f(-0.13f, 0.1f, 0.0f);	//3

	glVertex3f(-0.13f, 0.1f, 0.0f);	//3
	glVertex3f(-0.03f, 0.1f, 0.0f);	//4

	glVertex3f(-0.03f, 0.1f, 0.0f);	//4
	glVertex3f(0.06f, 0.3f, 0.0f);	//1


	glVertex3f(0.06f, 0.3f, 0.0f);	//1
	glVertex3f(-0.06f, 0.3f, 0.0f);	//2

	glVertex3f(-0.06f, 0.3f, 0.0f);	//2
	glVertex3f(0.03f, 0.1f, 0.0f);	//6

	glVertex3f(0.03f, 0.1f, 0.0f);	//6
	glVertex3f(0.13f, 0.1f, 0.0f);	//7

	glVertex3f(0.13f, 0.1f, 0.0f);	//7
	glVertex3f(0.06f, 0.3f, 0.0f);	//1

	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.13f, 0.1f, 0.0f);	//3
	glVertex3f(-0.19f, -0.1f, 0.0f); //8

	glVertex3f(-0.19f, -0.1f, 0.0f); //8
	glVertex3f(-0.09f, -0.1f, 0.0f);  //9

	glVertex3f(-0.09f, -0.1f, 0.0f);  //9
	glVertex3f(-0.03f, 0.1f, 0.0f);	//4

	glVertex3f(-0.03f, 0.1f, 0.0f);	//4
	glVertex3f(-0.13f, 0.1f, 0.0f);	//3


	glVertex3f(0.03f, 0.1f, 0.0f);	//6
	glVertex3f(0.09f, -0.1f, 0.0f);	//10

	glVertex3f(0.09f, -0.1f, 0.0f);	//10
	glVertex3f(0.19f, -0.1f, 0.0f);	//11

	glVertex3f(0.19f, -0.1f, 0.0f);	//11
	glVertex3f(0.13f, 0.1f, 0.0f);	//7

	glVertex3f(0.13f, 0.1f, 0.0f);	//7
	glVertex3f(0.03f, 0.1f, 0.0f);	//6


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(-0.19f, -0.1f, 0.0f); //8
	glVertex3f(-0.25f, -0.3f, 0.0f);	//12

	glVertex3f(-0.25f, -0.3f, 0.0f);	//12
	glVertex3f(-0.15f, -0.3f, 0.0f);	//13

	glVertex3f(-0.15f, -0.3f, 0.0f);	//13
	glVertex3f(-0.09f, -0.1f, 0.0f);  //9

	glVertex3f(-0.09f, -0.1f, 0.0f);  //9
	glVertex3f(-0.19f, -0.1f, 0.0f); //8


	glVertex3f(0.09f, -0.1f, 0.0f);	//10
	glVertex3f(0.15f, -0.3f, 0.0f);	//14

	glVertex3f(0.15f, -0.3f, 0.0f);	//14
	glVertex3f(0.25f, -0.3f, 0.0f);	//15

	glVertex3f(0.25f, -0.3f, 0.0f);	//15
	glVertex3f(0.19f, -0.1f, 0.0f);	//11

	glVertex3f(0.19f, -0.1f, 0.0f);	//11
	glVertex3f(0.09f, -0.1f, 0.0f);	//10



	if (axis >= 5.8f)
	{
		//////////////////////////////////////////////////////
		glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
		glVertex3f(0.05f, 0.030f, 0.0f);	//a
		glVertex3f(-0.05f, 0.030f, 0.0f);	//b

		glVertex3f(-0.05f, 0.030f, 0.0f);	//b
		glVertex3f(-0.057f, 0.010f, 0.0f);	//c

		glVertex3f(-0.057f, 0.010f, 0.0f);	//c
		glVertex3f(0.057f, 0.010f, 0.0f);	//d

		glVertex3f(0.057f, 0.010f, 0.0f);	//d
		glVertex3f(0.05f, 0.030f, 0.0f);	//a


		glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

		glVertex3f(0.057f, 0.010f, 0.0f);	//d
		glVertex3f(-0.057f, 0.010f, 0.0f);	//c

		glVertex3f(-0.057f, 0.010f, 0.0f);	//c
		glVertex3f(-0.063f, -0.010f, 0.0f);	//f

		glVertex3f(-0.063f, -0.010f, 0.0f);	//f
		glVertex3f(0.063f, -0.010f, 0.0f);	//e

		glVertex3f(0.063f, -0.010f, 0.0f);	//e
		glVertex3f(0.056f, 0.010f, 0.0f);	//d


		glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR
		glVertex3f(0.063f, -0.010f, 0.0f);	//e
		glVertex3f(-0.063f, -0.010f, 0.0f);	//f

		glVertex3f(-0.063f, -0.010f, 0.0f);	//f
		glVertex3f(-0.069f, -0.030f, 0.0f);	//g

		glVertex3f(-0.069f, -0.030f, 0.0f);	//g
		glVertex3f(0.069f, -0.030f, 0.0f);	//h

		glVertex3f(0.069f, -0.030f, 0.0f);	//h
		glVertex3f(0.063f, -0.010f, 0.0f);	//e
	}


	glEnd();

	if (NegXAxisForA >= 0.9f)
	{
		NegXAxisForA -= AxisChange;

	}
	else
	{
		NegXAxisForA = 0.9f;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End Of A
	////////////////////////////////////////////////////////////////////////////////////////////////////////


	if (NegYAxisForI >= -0.2f)
	{

		/////////Stright PLANE///////////////////Stright PLANE//////////////////Stright PLANE///////////////Stright PLANE//////////////////////////Stright PLANE/////////////
		static GLfloat planeAxisChange = 0.011f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (axis <= 15.0f)
		{
			glTranslatef(axis, 0.0f, -6.0f);
		}

		glBegin(GL_QUADS);

		flag();
		IAFPlane();

		glEnd();

		axis += planeAxisChange;
		if (axis >= 12.0f)
		{
			axis = -10.0f;
		}

		/////////Top PLANE///////////////////Top PLANE//////////////////Top PLANE///////////////Top PLANE//////////////////////////Top PLANE/////////////

		static GLfloat topXAxis = -10.0f;
		static GLfloat topYAxis = 5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (topXAxis <= 12.0f)
		{
			glTranslatef(topXAxis, topYAxis, -6.0f);
		}

		glBegin(GL_QUADS);

		flag();
		IAFPlane();

		glEnd();

		topXAxis += planeAxisChange;
		if (topXAxis >= 12.0f)
		{
			topXAxis = -10.0f;
		}

		if (topXAxis >= 6.5f)
		{
			topYAxis += planeAxisChange;
		}

		else if (topYAxis <= 0.0f)
		{
			topYAxis = 0.0f;
		}
		else
		{
			topYAxis -= planeAxisChange;
		}

		/////////BOTTOM PLANE///////////////////BOTTOM PLANE//////////////////BOTTOM PLANE///////////////BOTTOM PLANE//////////////////////////BOTTOM PLANE/////////////

		static GLfloat bottomXAxis = -10.0f;
		static GLfloat bottomYAxis = -5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (bottomXAxis <= 12.0f)
		{
			glTranslatef(bottomXAxis, bottomYAxis, -6.0f);
		}

		glBegin(GL_QUADS);
		flag();
		IAFPlane();
		glEnd();

		bottomXAxis += planeAxisChange;
		if (bottomXAxis >= 12.0f)
		{
			bottomXAxis = -10.0f;
		}

		if (bottomXAxis >= 6.5f)
		{
			bottomYAxis -= planeAxisChange;
		}

		else if (bottomYAxis >= 0.0f)
		{
			bottomYAxis = 0.0f;
		}
		else
		{
			bottomYAxis += planeAxisChange;
		}



		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IAF word on Top Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static GLfloat topXAxisIAF = -10.0f;
		static GLfloat topYAxisIAF = 5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (topXAxisIAF <= 12.0f)
		{
			glTranslatef(topXAxisIAF, topYAxisIAF, -6.0f);
		}

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);		//WHITE COLOR

		callIAF();

		glEnd();
		topXAxisIAF += planeAxisChange;
		if (topXAxisIAF >= 12.0f)
		{
			topXAxisIAF = -10.0f;
		}

		if (topXAxisIAF >= 6.5f)
		{
			topYAxisIAF += planeAxisChange;
		}

		else if (topYAxisIAF <= 0.0f)
		{
			topYAxisIAF = 0.0f;
		}
		else
		{
			topYAxisIAF -= planeAxisChange;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// End Of Top IAF on Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IAF word on straight Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static GLfloat axis_i = -10.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (axis_i <= 15.0f)
		{
			glTranslatef(axis_i, 0.0f, -6.0f);
		}

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);		//WHITE COLOR

		callIAF();

		glEnd();
		axis_i += planeAxisChange;
		if (axis_i >= 12.0f)
		{
			axis_i = -10.0f;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Straight	End of IAF on Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IAF word on Bottom Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static GLfloat bottomXAxisI = -10.0f;
		static GLfloat bottomYAxisI = -5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (bottomXAxisI <= 15.0f)
		{
			glTranslatef(bottomXAxisI, bottomYAxisI, -6.0f);
		}

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);		//WHITE COLOR

		callIAF();

		glEnd();

		bottomXAxisI += planeAxisChange;
		if (bottomXAxisI >= 12.0f)
		{
			bottomXAxisI = -10.0f;
		}

		if (bottomXAxisI >= 6.5f)
		{
			bottomYAxisI -= planeAxisChange;
		}
		else if (bottomYAxisI >= 0.0f)
		{
			bottomYAxisI = 0.0f;
		}
		else
		{
			bottomYAxisI += planeAxisChange;
		}

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of Bottom of IAF on Plane
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	glEnd();



	SwapBuffers(ghdc);
}

void IAFPlane()
{
	static long int iNum = 5000000;


	glColor3f(0.3f, 0.3f, 0.3f);	//Gray Color
	glVertex3f(1.0f, 0.0f, 0.0f);	//1
	glVertex3f(0.7f, 0.1f, 0.0f);	//2


	glVertex3f(0.7f, 0.1f, 0.0f);	//2
	glVertex3f(0.7f, 0.0f, 0.0f);	//38

	glVertex3f(0.7f, 0.0f, 0.0f);	//38
	glVertex3f(0.7f, -0.1f, 0.0f);	//34

	glVertex3f(0.7f, -0.1f, 0.0f);	//34
	glVertex3f(1.0f, 0.0f, 0.0f);	//1

	glColor3f(0.6f, 0.6f, 0.6f);	//Light gray color
	glVertex3f(0.7f, 0.1f, 0.0f);	//2
	glVertex3f(0.4f, 0.15f, 0.0f);	//3

	glVertex3f(0.4f, 0.15f, 0.0f);	//3
	glVertex3f(0.4f, -0.15f, 0.0f);	//33

	glVertex3f(0.4f, -0.15f, 0.0f);	//33
	glVertex3f(0.7f, -0.1f, 0.0f);	//34

	glVertex3f(0.7f, -0.1f, 0.0f);	//34
	glVertex3f(0.7f, 0.1f, 0.0f);	//2




	glVertex3f(0.4f, 0.15f, 0.0f);	//3
	glVertex3f(0.2f, 0.3f, 0.0f);	//4

	glVertex3f(0.2f, 0.3f, 0.0f);	//4
	glVertex3f(0.15f, 0.3f, 0.0f);	//5

	glVertex3f(0.15f, 0.3f, 0.0f);	//5
	glVertex3f(0.2f, 0.15f, 0.0f);	//6

	glVertex3f(0.2f, 0.15f, 0.0f);	//6
	glVertex3f(0.4f, 0.15f, 0.0f);	//3


	glVertex3f(0.4f, 0.15f, 0.0f);	//3
	glVertex3f(0.2f, 0.15f, 0.0f);	//6

	glVertex3f(0.2f, 0.15f, 0.0f);	//6
	glVertex3f(0.2f, -0.15f, 0.0f);	//30

	glVertex3f(0.2f, -0.15f, 0.0f);	//30
	glVertex3f(0.4f, -0.15f, 0.0f);	//33

	glVertex3f(0.4f, -0.15f, 0.0f);	//33
	glVertex3f(0.4f, 0.15f, 0.0f);	//3




	glVertex3f(0.4f, -0.15f, 0.0f);	//33
	glVertex3f(0.2f, -0.15f, 0.0f);	//30

	glVertex3f(0.2f, -0.15f, 0.0f);	//30
	glVertex3f(0.15f, -0.3f, 0.0f);	//31

	glVertex3f(0.15f, -0.3f, 0.0f);	//31
	glVertex3f(0.2f, -0.3f, 0.0f);	//32

	glVertex3f(0.2f, -0.3f, 0.0f);	//32
	glVertex3f(0.4f, -0.15f, 0.0f);	//33



	glVertex3f(0.2f, 0.15f, 0.0f);	//6
	glVertex3f(0.15f, 0.17f, 0.0f);	//7

	glVertex3f(0.15f, 0.17f, 0.0f);	//7
	glVertex3f(0.15f, -0.17f, 0.0f);	//29

	glVertex3f(0.15f, -0.17f, 0.0f);	//29
	glVertex3f(0.2f, -0.15f, 0.0f);	//30

	glVertex3f(0.2f, -0.15f, 0.0f);	//30
	glVertex3f(0.2f, 0.15f, 0.0f);	//6



	glVertex3f(0.15f, 0.17f, 0.0f);	//7
	glVertex3f(-0.3f, 0.6f, 0.0f);	//8

	glVertex3f(-0.3f, 0.6f, 0.0f);	//8
	glVertex3f(-0.5f, 0.6f, 0.0f);	//9

	glVertex3f(-0.5f, 0.6f, 0.0f);	//9
	glVertex3f(-0.3f, 0.15f, 0.0f);	//10

	glVertex3f(-0.3f, 0.15f, 0.0f);	//10
	glVertex3f(0.15f, 0.17f, 0.0f);	//7


	glVertex3f(0.15f, 0.17f, 0.0f);	//7
	glVertex3f(-0.3f, 0.15f, 0.0f);	//10
	
	glVertex3f(-0.3f, 0.15f, 0.0f);	//10
	glVertex3f(-0.3f, -0.15f, 0.0f);	//26


	glVertex3f(-0.3f, -0.15f, 0.0f);	//26
	glVertex3f(0.15f, -0.17f, 0.0f);	//29

	glVertex3f(0.15f, -0.17f, 0.0f);	//29
	glVertex3f(0.15f, 0.17f, 0.0f);	//7



	glVertex3f(-0.3f, -0.15f, 0.0f);	//26
	glVertex3f(-0.5f, -0.6f, 0.0f);	//27

	glVertex3f(-0.5f, -0.6f, 0.0f);	//27
	glVertex3f(-0.3f, -0.6f, 0.0f);	//28

	glVertex3f(-0.3f, -0.6f, 0.0f);	//28
	glVertex3f(0.15f, -0.17f, 0.0f);	//29

	glVertex3f(0.15f, -0.17f, 0.0f);	//29
	glVertex3f(-0.3f, -0.15f, 0.0f);	//26


	glVertex3f(-0.35f, 0.15f, 0.0f);	//12
	glVertex3f(-0.7f, 0.4f, 0.0f);	//13

	glVertex3f(-0.7f, 0.4f, 0.0f);	//13
	glVertex3f(-0.8f, 0.35f, 0.0f);	//14

	glVertex3f(-0.8f, 0.35f, 0.0f);	//14
	glVertex3f(-0.75f, 0.15f, 0.0f);	//15

	glVertex3f(-0.75f, 0.15f, 0.0f);	//15
	glVertex3f(-0.35f, 0.15f, 0.0f);	//12



	glColor3f(0.3f, 0.3f, 0.3f);	//Gray Color

	glVertex3f(-0.25f, 0.15f, 0.0f);	//11
	glVertex3f(-0.8f, 0.15f, 0.0f);	//16

	glVertex3f(-0.8f, 0.15f, 0.0f);	//16
	glVertex3f(-0.8f, 0.05f, 0.0f);	//17

	glVertex3f(-0.8f, 0.05f, 0.0f);	//17
	glVertex3f(-0.25f, 0.05f, 0.0f);	//36

	glVertex3f(-0.25f, 0.05f, 0.0f);	//36
	glVertex3f(-0.25f, 0.15f, 0.0f);	//11



	glColor3f(0.6f, 0.6f, 0.6f);	//Light gray color
	glVertex3f(-0.25f, 0.05f, 0.0f);	//36
	glVertex3f(-0.9f, 0.05f, 0.0f);	//18

	glVertex3f(-0.9f, 0.05f, 0.0f);	//18
	glVertex3f(-0.9f, -0.05f, 0.0f);	//19

	glVertex3f(-0.9f, -0.05f, 0.0f);	//19
	glVertex3f(-0.25f, -0.05f, 0.0f);	//37

	glVertex3f(-0.25f, -0.05f, 0.0f);	//37
	glVertex3f(-0.25f, 0.05f, 0.0f);	//36


	glColor3f(0.3f, 0.3f, 0.3f);	//Gray Color
	glVertex3f(-0.25f, -0.05f, 0.0f);	//37
	glVertex3f(-0.8f, -0.05f, 0.0f);	//20

	glVertex3f(-0.8f, -0.05f, 0.0f);	//20
	glVertex3f(-0.8f, -0.15f, 0.0f);	//21

	glVertex3f(-0.8f, -0.15f, 0.0f);	//21
	glVertex3f(-0.25f, -0.15f, 0.0f);	//25

	glVertex3f(-0.25f, -0.15f, 0.0f);	//25
	glVertex3f(-0.25f, -0.05f, 0.0f);	//37


	glColor3f(0.6f, 0.6f, 0.6f);	//Light gray color
	glVertex3f(-0.35f, -0.15f, 0.0f);	//24
	glVertex3f(-0.75f, -0.15f, 0.0f);	//35

	glVertex3f(-0.75f, -0.15f, 0.0f);	//35
	glVertex3f(-0.8f, -0.35f, 0.0f);	//22

	glVertex3f(-0.8f, -0.35f, 0.0f);	//22
	glVertex3f(-0.7f, -0.4f, 0.0f);	//23

	glVertex3f(-0.7f, -0.4f, 0.0f);	//23
	glVertex3f(-0.35f, -0.15f, 0.0f);	//24




	if ((iNum % 2) == 0)
	{
		////////////////////////////////////////////////TOP LIGHT////////////////////////////////////////
		glColor3f(1.0f, 0.0f, 0.0f);	//RED
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8
		glVertex3f(-0.23f, 0.6f, 0.0f);	//50

		glVertex3f(-0.23f, 0.6f, 0.0f);	//50
		glVertex3f(-0.23f, 0.57f, 0.0f);	//51

		glVertex3f(-0.23f, 0.57f, 0.0f);	//51
		glVertex3f(-0.28f, 0.57f, 0.0f);	//52

		glVertex3f(-0.28f, 0.57f, 0.0f);	//52
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8


		glColor3f(0.0f, 0.0f, 1.0f);	//BLUE
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58
		glVertex3f(-0.17f, 0.55f, 0.0f);	//55

		glVertex3f(-0.17f, 0.55f, 0.0f);	//55
		glVertex3f(-0.17f, 0.51f, 0.0f);	//56

		glVertex3f(-0.17f, 0.51f, 0.0f);	//56
		glVertex3f(-0.2f, 0.51f, 0.0f);	//57

		glVertex3f(-0.2f, 0.51f, 0.0f);	//57
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58



		////////////////////////////////////////////////Bottom LIGHT////////////////////////////////////////

		glColor3f(1.0f, 0.0f, 0.0f);	//RED
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71	
		glVertex3f(-0.28f, -0.57f, 0.0f);	//72

		glVertex3f(-0.28f, -0.57f, 0.0f);	//72
		glVertex3f(-0.3f, -0.6f, 0.0f);	//28

		glVertex3f(-0.3f, -0.6f, 0.0f);	//28
		glVertex3f(-0.23f, -0.6f, 0.0f);	//70

		glVertex3f(-0.23f, -0.6f, 0.0f);	//70
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71




		glColor3f(0.0f, 0.0f, 1.0f);	//BLUE
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61
		glVertex3f(-0.17f, -0.51f, 0.0f);	//60

		glVertex3f(-0.17f, -0.51f, 0.0f);	//60
		glVertex3f(-0.17f, -0.55f, 0.0f);	//63

		glVertex3f(-0.17f, -0.55f, 0.0f);	//63
		glVertex3f(-0.25f, -0.55f, 0.0f);	//62

		glVertex3f(-0.25f, -0.55f, 0.0f);	//62
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61
	}
	else
	{
		////////////////////////////////////////////////TOP LIGHT////////////////////////////////////////
		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8
		glVertex3f(-0.23f, 0.6f, 0.0f);	//50

		glVertex3f(-0.23f, 0.6f, 0.0f);	//50
		glVertex3f(-0.23f, 0.57f, 0.0f);	//51

		glVertex3f(-0.23f, 0.57f, 0.0f);	//51
		glVertex3f(-0.28f, 0.57f, 0.0f);	//52

		glVertex3f(-0.28f, 0.57f, 0.0f);	//52
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8


		glColor3f(1.0f, 1.0f, 1.0f);	//WHITE
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58
		glVertex3f(-0.17f, 0.55f, 0.0f);	//55

		glVertex3f(-0.17f, 0.55f, 0.0f);	//55
		glVertex3f(-0.17f, 0.51f, 0.0f);	//56

		glVertex3f(-0.17f, 0.51f, 0.0f);	//56
		glVertex3f(-0.2f, 0.51f, 0.0f);	//57

		glVertex3f(-0.2f, 0.51f, 0.0f);	//57
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58



		////////////////////////////////////////////////Bottom LIGHT////////////////////////////////////////

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71	
		glVertex3f(-0.28f, -0.57f, 0.0f);	//72

		glVertex3f(-0.28f, -0.57f, 0.0f);	//72
		glVertex3f(-0.3f, -0.6f, 0.0f);	//28

		glVertex3f(-0.3f, -0.6f, 0.0f);	//28
		glVertex3f(-0.23f, -0.6f, 0.0f);	//70

		glVertex3f(-0.23f, -0.6f, 0.0f);	//70
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71




		glColor3f(1.0f, 1.0f, 1.0f);	//WHITE
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61
		glVertex3f(-0.17f, -0.51f, 0.0f);	//60

		glVertex3f(-0.17f, -0.51f, 0.0f);	//60
		glVertex3f(-0.17f, -0.55f, 0.0f);	//63

		glVertex3f(-0.17f, -0.55f, 0.0f);	//63
		glVertex3f(-0.25f, -0.55f, 0.0f);	//62

		glVertex3f(-0.25f, -0.55f, 0.0f);	//62
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61


	}
	iNum -= 1;


	//glColor3f(0.2f, 0.8f, 1.0f);

		//glVertex3f(0.7f, 0.1f, 0.0f);	//1
		//glVertex3f(0.3f, 0.1f, 0.0f);	//2

		//glVertex3f(0.3f, 0.1f, 0.0f);	//2
		//glVertex3f(0.3f, -0.1f, 0.0f);	//23f

		//glVertex3f(0.3f, -0.1f, 0.0f);	//23
		//glVertex3f(0.7f, -0.1f, 0.0f);	//24

		//glVertex3f(0.7f, -0.1f, 0.0f);	//24
		//glVertex3f(0.7f, 0.1f, 0.0f);	//1





		//glVertex3f(0.3f, 0.2f, 0.0f);	//3
		//glVertex3f(0.15f, 0.2f, 0.0f);	//4

		//glVertex3f(0.15f, 0.2f, 0.0f);	//4
		//glVertex3f(0.15f, -0.2f, 0.0f);	//21

		//glVertex3f(0.15f, -0.2f, 0.0f);	//21
		//glVertex3f(0.3f, -0.2f, 0.0f);	//22

		//glVertex3f(0.3f, -0.2f, 0.0f);	//22
		//glVertex3f(0.3f, 0.2f, 0.0f);	//3



		//glVertex3f(0.15f, 0.2f, 0.0f);	//4
		//glVertex3f(-0.6f, 0.7f, 0.0f);	//5

		//glVertex3f(-0.6f, 0.7f, 0.0f);	//5
		//glVertex3f(-0.8f, 0.6f, 0.0f);	//6

		//glVertex3f(-0.8f, 0.6f, 0.0f);	//6
		//glVertex3f(-0.5f, 0.2f, 0.0f);	//7

		//glVertex3f(-0.5f, 0.2f, 0.0f);	//7
		//glVertex3f(0.15f, 0.2f, 0.0f);	//4




		//glVertex3f(-0.5f, 0.2f, 0.0f);	//7
		//glVertex3f(0.15f, 0.2f, 0.0f);	//4

		//glVertex3f(0.15f, 0.2f, 0.0f);	//4
		//glVertex3f(0.15f, -0.2f, 0.0f);	//21

		//glVertex3f(0.15f, -0.2f, 0.0f);	//21
		//glVertex3f(-0.5f, -0.2f, 0.0f);	//18

		//glVertex3f(-0.5f, -0.2f, 0.0f);	//18
		//glVertex3f(-0.5f, 0.2f, 0.0f);	//7






		//glVertex3f(0.15f, -0.2f, 0.0f);	//21
		//glVertex3f(-0.5f, -0.2f, 0.0f);	//18

		//glVertex3f(-0.5f, -0.2f, 0.0f);	//18
		//glVertex3f(-0.8f, -0.6f, 0.0f);	//19

		//glVertex3f(-0.8f, -0.6f, 0.0f);	//19
		//glVertex3f(-0.6f, -0.7f, 0.0f);	//20

		//glVertex3f(-0.6f, -0.7f, 0.0f);	//20
		//glVertex3f(0.15f, -0.2f, 0.0f);	//21





		//glVertex3f(-0.5f, 0.2f, 0.0f);	//7
		//glVertex3f(-0.6f, 0.2f, 0.0f);	//8

		//glVertex3f(-0.6f, 0.2f, 0.0f);	//8
		//glVertex3f(-0.6f, -0.2f, 0.0f);	//17

		//glVertex3f(-0.6f, -0.2f, 0.0f);	//17
		//glVertex3f(-0.5f, -0.2f, 0.0f);	//18

		//glVertex3f(-0.5f, -0.2f, 0.0f);	//18
		//glVertex3f(-0.5f, 0.2f, 0.0f);	//7





		//glVertex3f(-0.6f, 0.2f, 0.0f);	//8
		//glVertex3f(-0.9f, 0.45f, 0.0f);	//9

		//glVertex3f(-0.9f, 0.45f, 0.0f);	//9
		//glVertex3f(-1.0f, 0.45f, 0.0f);	//10

		//glVertex3f(-1.0f, 0.45f, 0.0f);	//10
		//glVertex3f(-0.85f, 0.15f, 0.0f);	//11

		//glVertex3f(-0.85f, 0.15f, 0.0f);	//11
		//glVertex3f(-0.6f, 0.2f, 0.0f);	//8





		//glVertex3f(-0.6f, 0.2f, 0.0f);	//8
		//glVertex3f(-0.85f, 0.15f, 0.0f);	//11

		//glVertex3f(-0.85f, 0.15f, 0.0f);	//11
		//glVertex3f(-0.85f, -0.15f, 0.0f);	//14

		//glVertex3f(-0.85f, -0.15f, 0.0f);	//14
		//glVertex3f(-0.6f, -0.2f, 0.0f);	//17

		//glVertex3f(-0.6f, -0.2f, 0.0f);	//17
		//glVertex3f(-0.6f, 0.2f, 0.0f);	//8





		//glVertex3f(-0.85f, -0.15f, 0.0f);	//14
		//glVertex3f(-0.6f, -0.2f, 0.0f);	//17

		//glVertex3f(-0.6f, -0.2f, 0.0f);	//17
		//glVertex3f(-0.9f, -0.45f, 0.0f);	//16

		//glVertex3f(-0.9f, -0.45f, 0.0f);	//16
		//glVertex3f(-1.0f, -0.45f, 0.0f);	//15

		//glVertex3f(-1.0f, -0.45f, 0.0f);	//15
		//glVertex3f(-0.85f, -0.15f, 0.0f);	//14




		//glVertex3f(-0.85f, 0.15f, 0.0f);	//11
		//glVertex3f(-0.9f, 0.1f, 0.0f);	//12

		//glVertex3f(-0.9f, 0.1f, 0.0f);	//12
		//glVertex3f(-0.9f, -0.1f, 0.0f);	//13

		//glVertex3f(-0.9f, -0.1f, 0.0f);	//13
		//glVertex3f(-0.85f, -0.15f, 0.0f);	//14

		//glVertex3f(-0.85f, -0.15f, 0.0f);	//14
		//glVertex3f(-0.85f, 0.15f, 0.0f);	//11



		//glVertex3f(0.7f, 0.1f, 0.0f);	//1
		//glVertex3f(0.7f, -0.1f, 0.0f);	//24

		//glVertex3f(0.7f, -0.1f, 0.0f);	//24
		//glVertex3f(1.0f, 0.0f, 0.0f);	//25

		//glVertex3f(1.0f, 0.0f, 0.0f);	//25
		//glVertex3f(0.7f, 0.1f, 0.0f);	//1
}
void flag()
{
	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(-0.9f, 0.10f, 0.0f);	//26
	glVertex3f(-0.9f, 0.01f, 0.0f);	//27

	glVertex3f(-0.9f, 0.01f, 0.0f);	//27
	glVertex3f(-2.5f, 0.01f, 0.0f);	//31

	glVertex3f(-2.5f, 0.01f, 0.0f);	//31
	glVertex3f(-2.5f, 0.10f, 0.0f);	//30

	glVertex3f(-2.5f, 0.10f, 0.0f);	//30
	glVertex3f(-0.9f, 0.10f, 0.0f);	//26


	//WHITE
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.9f, 0.01f, 0.0f);	//27
	glVertex3f(-2.5f, 0.01f, 0.0f);	//31

	glVertex3f(-2.5f, 0.01f, 0.0f);	//31
	glVertex3f(-2.5f, -0.07f, 0.0f);	//32

	glVertex3f(-2.5f, -0.07f, 0.0f);	//32
	glVertex3f(-0.9f, -0.07f, 0.0f);	//28

	glVertex3f(-0.9f, -0.07f, 0.0f);	//28
	glVertex3f(-0.9f, 0.01f, 0.0f);	//27

	//GREEN
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.9f, -0.07f, 0.0f);	//28
	glVertex3f(-2.5f, -0.07f, 0.0f);	//32

	glVertex3f(-2.5f, -0.07f, 0.0f);	//32
	glVertex3f(-2.5f, -0.14f, 0.0f);	//33

	glVertex3f(-2.5f, -0.14f, 0.0f);	//33
	glVertex3f(-0.9f, -0.14f, 0.0f);	//29

	glVertex3f(-0.9f, -0.14f, 0.0f);	//29
	glVertex3f(-0.9f, -0.07f, 0.0f);	//28
}
void callIAF()
{
	// I
	glVertex3f(0.0f, 0.1f, 0.0f);
	glVertex3f(0.0f, -0.1f, 0.0f);

	// A
	glVertex3f(0.1f, 0.1f, 0.0f);
	glVertex3f(0.03f, -0.1f, 0.0f);

	glVertex3f(0.1f, 0.1f, 0.0f);
	glVertex3f(0.17f, -0.1f, 0.0f);

	glVertex3f(0.05f, -0.03f, 0.0f);
	glVertex3f(0.15f, -0.03f, 0.0f);

	// F
	glVertex3f(0.21f, 0.1f, 0.0f);
	glVertex3f(0.21f, -0.1f, 0.0f);

	glVertex3f(0.21f, 0.09f, 0.0f);
	glVertex3f(0.31f, 0.09f, 0.0f);

	glVertex3f(0.21f, 0.02f, 0.0f);
	glVertex3f(0.27f, 0.02f, 0.0f);
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

void circle(void)
{

}