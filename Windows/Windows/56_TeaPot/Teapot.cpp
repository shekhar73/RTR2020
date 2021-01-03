// header files
#include<windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include"TeapotHeader.h"
#include"Teapot.h"

// macros
#pragma comment (lib, "OPENGL32.LIB")
#pragma comment (lib, "GLU32.LIB")
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global variable
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen = false;
HWND ghwnd = NULL;
FILE* gpFile = NULL;
bool gbActiveWindow = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
GLuint marbleTexture;
bool gbTexture = false;
bool gbLight = false;

GLfloat lightAmbiant[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// grey light
GLfloat lightDefuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };	// light from +z to -z

GLfloat materialAmbiant[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// grey light
GLfloat materialDefuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat materialShinyness = 128.0f;

static GLfloat angle = 0.0f;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declarations
	void Initialize(void);
	void Display(void);

	// Local variables
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	bool bDone = false;

	// code
	if (fopen_s(&gpFile, "SOMSHEKHAR.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Can not create desired file!"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("TeaPot"),
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

		// Game Loop
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
	// function declaration
	void ToggleFullScreen(void);
	void Resize(int, int);
	void UnInitialize(void);
	void Animate(void);

	// local variable

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		case 'L':
		case 'l':
			if (gbLight == false)
			{
				glEnable(GL_LIGHTING);
				gbLight = true;
			}
			else
			{
				glDisable(GL_LIGHTING);
				gbLight = false;
			}
			break;

		case 'A':
		case 'a':
			angle += 1.0f;
			if (angle == 360.0f)
				angle = 0.0f;
			break;

		case 'T':
		case 't':
			if (gbTexture == false)
			{
				glEnable(GL_TEXTURE_2D);
				gbTexture = true;
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				gbTexture = false;
			}
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		UnInitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}
	
void ToggleFullScreen()
{
	// function declaration

	// local variable
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && (GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
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
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void Initialize()
{
	// function declaration
	void Resize(int, int);
	bool LoadGLTexture(GLuint*, TCHAR[]);

	// local variable
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
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
		fprintf(gpFile, "ChoosePixelFormat() Failed");
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() Failed");
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() Failed");
		DestroyWindow(ghwnd);
	}
	
	//	glShadeModel();

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	LoadGLTexture(&marbleTexture, MAKEINTRESOURCE(MARBLE_BITMAP));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbiant);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDefuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbiant);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShinyness);

	glEnable(GL_LIGHT0);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

bool LoadGLTexture(GLuint* texture, TCHAR resourceID[])
{
	// local variable
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	// code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// push data into graphic memory with help of graphics driver
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);	// (gluBuild2DMipmaps() = glText2d() + glGenerateBitMap())
		DeleteObject(hBitmap);
	}
	return bResult;
}

void Resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void Display()
{
	// function declaration

	// local variable

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.0f);
	glBindTexture(GL_TEXTURE_2D, marbleTexture);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < sizeof(face_indicies) / sizeof(face_indicies[0]); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int vi = face_indicies[i][j];
			int ni = face_indicies[i][j + 3];
			int ti = face_indicies[i][j + 6];

			glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);
			glTexCoord2f(textures[ti][0], textures[ti][1]);
			glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
		}
	}

	glEnd();

	SwapBuffers(ghdc);
}

void UnInitialize()
{
	// function declaration

	// local variable

	// code
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
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

	if (gpFile)
	{
		fclose(gpFile);
		gpFile = NULL;
	}

	glDeleteTextures(1, &marbleTexture);
}
