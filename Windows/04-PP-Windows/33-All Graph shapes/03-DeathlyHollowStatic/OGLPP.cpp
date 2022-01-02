#include <windows.h>
#include <stdio.h>

#include <gl/glew.h>
#include <gl/GL.h>

#include "vmath.h"	// It is a C++ file

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

// we will give these values to the VERTEX which have position, color, normal, texture,
// velocity, gravity, viscocity, shineness, damping, blurring...more properties
enum
{
	SSK_ATTRIBUTE_POSITION	= 0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE
};

// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
FILE *gpFile			=	NULL;
HWND ghwnd				= 	NULL;
HDC ghdc				=	NULL;
HGLRC ghrc				= 	NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev	= { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow			= false;
bool gbEscapeKeyIsPressed	= false;
bool gbFullScreen			= false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Graph;
GLuint gVbo_Graph_Vertex;
GLuint gVbo_Graph_Color;

GLuint gVao_Axis;
GLuint gVbo_Axis_Vertex;
GLuint gVbo_Axis_Color;

GLuint gVao_Rectangle;
GLuint gVbo_Rectangle_Vertex;
GLuint gVbo_Rectangle_Color;

GLuint gVao_Triangle;
GLuint gVbo_Triangle_Vertex;
GLuint gVbo_Triangle_Color;

GLuint gVao_First_Circle;
GLuint gVbo_First_Circle_Vertex;
GLuint gVbo_First_Circle_Color;

GLuint gVao_Second_Circle;
GLuint gVbo_Second_Circle_Vertex;
GLuint gVbo_Second_Circle_Color;

GLfloat gCenterX;
GLfloat gCenterY;

GLuint mvpMatrixUniform;

// It is a matrix of 4 X 4 which is declared in vmath.h header file.
// It will be used for transformation
//mat4 gPerspectiveProjectionMatrix; // vmath::mat4 gPerspectiveProjectionMatrix
mat4 gPerspectiveProjectionMatrix;

// Entry Point function i.e. main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Prototype
	void Initialize(void);
	void UnInitialize(void);
	void Display(void);

	// Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGLPP");
	bool bDone = false;

	// code
	
	if (fopen_s(&gpFile, "LogFile.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}

	wndclass.cbSize			= sizeof(WNDCLASSEX);
	wndclass.style 			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance 		= hInstance;
	wndclass.hbrBackground 	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon 			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor 		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm 		= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc 	= WndProc;
	wndclass.lpszClassName	= szClassName;
	wndclass.lpszMenuName 	= NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable Pipeline : Window"),
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

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	Initialize();

	while(bDone == false)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
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
			Display();

			if(gbActiveWindow == true)
			{
				if(gbEscapeKeyIsPressed == true)
				{
					bDone = true;
				}
			}
		}
	}
	UnInitialize();

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void ToggleFullScreen(void);
	void Resize(int, int);
	void UnInitialize();

	// local variable

	// code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if(HIWORD(wParam) == 0)
		{
			gbActiveWindow = true;
		}
		else
		{
			gbActiveWindow = false;
		}
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
			if (gbEscapeKeyIsPressed == false)
			{
				gbEscapeKeyIsPressed = true;
			}
			break;

		case 0x46:
		case 0x66:
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;

		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CLOSE:
		UnInitialize();
		break;

	case WM_DESTROY:
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
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
		ShowCursor(TRUE);
	}
}

void Initialize(void)
{
	// Function prototypes
	void Resize(int, int);
	void UnInitialize();

	// Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ghdc = GetDC(ghwnd);
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// Initialization of structure 'PIXELFORMATDESCRIPTO'
	// Parallel to glutInitDisplayMode()
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

	ghdc = GetDC(ghwnd);

	// choos a pixel format which best matches with that of 'pfd'
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// set the pixel format chosen above
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// make the rendering above us current n the current hdc
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// GLEW Initialization code for GLSL (IMPORTANT : It must be here. 
	//Means after creating OpenGL context but before using any OpengGL function)
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//*** VERTEX SHADER ***
	//////////////////////////////////////////////////////////////////////////////////////

	// Create Shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// Provide source code to shader
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;"
		"out vec4 out_color;" \
		"void main(void)" \
		"\n" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;"
		"}";

	//gl_Position is inbuilt variable in shader
	// feed the source code to your shaderobject
	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	// shader compilation error checking

	//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	// glGetShaderiv � return a parameter from a shader object

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;

				// take log
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				//destroyWindow(ghwnd);
				UnInitialize(); //destroyWindow(ghwnd);-> call WND_DESTROY->PostQuitMessage()->MessageLoop()->uninitialize();
				exit(0);
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////
	//*** FRAGMENT SHADER ***
	///////////////////////////////////////////////////////////////////////////////////////

	// Create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"\n" \
		"{" \
		"FragColor = out_color;" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject);

	// fragment shader compilation error checking


	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//*** SHADER PROGRAM ***
	/////////////////////////////////////////////////////////////////////////////////////

	// create
	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	// pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes

	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");


	// link shader
	glLinkProgram(gShaderProgramObject);

	GLint iShaderProgramLinkStatus = 0;

	// Reset values
	iInfoLogLength = 0;
	szInfoLog = NULL;

	// shader link error checking
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);

	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetShaderiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}

	}

	// get MVP uniform location
	// post linking
	mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	// *** vertices, colors, shader attribs, vbo, vao initializations ***

	//---------------------GRAPH---------------------------------------------------------
	GLfloat graphVertices[42 * 2 * 3 * 2];

	GLfloat x = -1.0f;
	for (int i = 0; i < 41 * 2 * 3; i = i + 6)
	{
		graphVertices[i] = x;
		graphVertices[i + 1] = 1.0f;
		graphVertices[i + 2] = 0.0f;

		graphVertices[i + 3] = x;
		graphVertices[i + 4] = -1.0f;
		graphVertices[i + 5] = 0.0f;

		x = x + 0.05f;
	}

	GLfloat y = 1.0f;

	for (int i = 41 * 2 * 3; i < 41 * 2 * 3 * 2; i = i + 6)
	{
		graphVertices[i] = -1.0f;
		graphVertices[i + 1] = y;
		graphVertices[i + 2] = 0.0f;

		graphVertices[i + 3] = 1.0f;
		graphVertices[i + 4] = y;
		graphVertices[i + 5] = 0.0f;

		y = y - 0.05f;
	}

	GLfloat graphColors[42 * 2 * 3 * 2];
	for (int i = 0; i < 42 * 2 * 3 * 2; i = i + 3)
	{
		graphColors[i] = 0.0f;
		graphColors[i + 1] = 0.0f;
		graphColors[i + 2] = 1.0f;
	}


	// It will store all the below 6 steps so that we can reuse it in draw function
	glGenVertexArrays(1, &gVao_Graph);
	glBindVertexArray(gVao_Graph);
	glGenBuffers(1, &gVbo_Graph_Vertex);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Graph_Vertex);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(graphVertices), graphVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Graph_Color);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Graph_Color);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(graphColors), graphColors, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//---------------------End of GRAPH---------------------------------------------------------


	//----------------- AXIS of X and Y---------------------------------------------------------
	const GLfloat AxisVertices[] =
	{
		-1.0f,	0.0f,	0.0f,
		 1.0f,	0.0f,	0.0f,
		 0.0f,	1.0f,	0.0f,
		 0.0f, -1.0f,	0.0f
	};

	const GLfloat AxisColors[] =
	{
		1.0f,	0.0f,	0.0f,
		1.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,
		0.0f,	1.0f,	0.0f,
	};

	glGenVertexArrays(1, &gVao_Axis);
	glBindVertexArray(gVao_Axis);
	glGenBuffers(1, &gVbo_Axis_Vertex);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Axis_Vertex);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(AxisVertices), AxisVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Axis_Color);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Axis_Color);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(AxisColors), AxisColors, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//----------------- End of AXIS of X and Y-----------------------------------------------


	//------------------Rectangle------------------------------------------------------------

	const GLfloat rectVertices[] =
	{
		-1.0f,	0.75f,	0.0f,
		-1.0f, -0.75f,	0.0f,

		-1.0f, -0.75f,	0.0f,
		 1.0f,  -0.75f,	0.0f,

		 1.0f,  -0.75f,	0.0f,
		 1.0f,   0.75f,	0.0f,

		 1.0f,	0.75f,	0.0f,
		-1.0f,	0.75f,	0.0f
	};

	const GLfloat rectColors[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &gVao_Rectangle);
	glBindVertexArray(gVao_Rectangle);
	glGenBuffers(1, &gVbo_Rectangle_Vertex);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Rectangle_Vertex);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Rectangle_Color);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Rectangle_Color);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectColors), rectColors, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//------------------End of Rectangle------------------------------------------------------------

	//------------------Triangle------------------------------------------------------------
	GLfloat x1 = 0.0f;
	GLfloat y1 = 0.75f;

	GLfloat x2 = (cos(120.0f) * 0.75f);
	GLfloat y2 = -(sin(120.0f) * 0.75f);

	GLfloat x3 = -(cos(120.0f) * 0.75f);
	GLfloat y3 = -(sin(120.0f) * 0.75f);

	const GLfloat triangleVertices[] =
	{
		x1, y1, 0.0f,
		x2, y2, 0.0f,

		x2, y2, 0.0f,
		x3, y3, 0.0f,

		x3, y3, 0.0f,
		x1, y1, 0.0f,
	};

	GLfloat triangleColors[] =
	{
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &gVao_Triangle);
	glBindVertexArray(gVao_Triangle);
	glGenBuffers(1, &gVbo_Triangle_Vertex);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Triangle_Vertex);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Triangle_Color);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Triangle_Color);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//------------------End of Triangle------------------------------------------------------------


	//------------------Circle------------------------------------------------------------
	GLfloat radius = 0.75f;
	GLfloat circle1Vertices[360 * 9];
	GLfloat circle1Colors[360 * 9];

	GLfloat angle = 0.0f;
	for (int i = 0; i < 360 * 9; i = i + 3)
	{
		circle1Vertices[i] = radius * cos(angle);
		circle1Vertices[i + 1] = radius * sin(angle);
		circle1Vertices[i + 2] = 0.0f;

		angle = angle + 0.006f;

		circle1Colors[i] = 1.0f;
		circle1Colors[i + 1] = 0.05f;
		circle1Colors[i + 2] = 1.0f;
	}

	glGenVertexArrays(1, &gVao_First_Circle);
	glBindVertexArray(gVao_First_Circle);
	glGenBuffers(1, &gVbo_First_Circle_Vertex);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_First_Circle_Vertex);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle1Vertices), circle1Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_First_Circle_Color);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_First_Circle_Color);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle1Colors), circle1Colors, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	GLfloat a = sqrt(((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)));
	GLfloat b = sqrt(((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)));
	GLfloat c = sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
	
	gCenterX = ((a * x1) + (b * x2) + (c * x3)) / (a + b + c);
	gCenterY = ((a * y1) + (b * y2) + (c * y3)) / (a + b + c);
	
	GLfloat Perimeter = (a + b + c) / 2;
	GLfloat Area = sqrt(Perimeter * (Perimeter - a) * (Perimeter - b) * (Perimeter - c));
	GLfloat radiusInCircle = Area / Perimeter;

	GLfloat circle2Vertices[360 * 9];
	GLfloat circle2Colors[360 * 9];

	angle = 0.0f;
	for (int i = 0; i < 360 * 9; i = i + 3)
	{
		circle2Vertices[i] = radiusInCircle * cos(angle);
		circle2Vertices[i + 1] = radiusInCircle * sin(angle);
		circle2Vertices[i + 2] = 0.0f;

		angle = angle + 0.006f;

		circle2Colors[i] = 1.5f;
		circle2Colors[i + 1] = 0.05f;
		circle2Colors[i + 2] = 0.0f;
	}

	glGenVertexArrays(1, &gVao_Second_Circle);
		glBindVertexArray(gVao_Second_Circle);
		glGenBuffers(1, &gVbo_Second_Circle_Vertex);	//like glGenTextures() in FFP
		glBindBuffer(GL_ARRAY_BUFFER, gVbo_Second_Circle_Vertex);	// like glBindTexture() in FFP
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle2Vertices), circle2Vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &gVbo_Second_Circle_Color);	//like glGenTextures() in FFP
		glBindBuffer(GL_ARRAY_BUFFER, gVbo_Second_Circle_Color);	// like glBindTexture() in FFP
		glBufferData(GL_ARRAY_BUFFER, sizeof(circle2Colors), circle2Colors, GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//------------------End of Circle------------------------------------------------------------

	glShadeModel(GL_SMOOTH);
	// set-up detpth buffer
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// depth test to do
	glDepthFunc(GL_LEQUAL);
	// set really nice percpective calculations?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// we will always cull back faces for better performance
	glEnable(GL_CULL_FACE);

	// set background color to which it will display even if it will empty.
	// THIS LINE CAN BE IN drawRect().
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// blue

	// set PerspectiveProjectionMatrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	// resize
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object

	// Vertical Y-axis lines
	glUseProgram(gShaderProgramObject);

		mat4 translateMatrix = mat4::identity();
		mat4 modelViewMatrix = mat4::identity();
		mat4 modelViewProjectionMatrix = mat4::identity();
	
		translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
		modelViewMatrix = translateMatrix;
		modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		//--------------------- Graph------------------------------------------------------

		glBindVertexArray(gVao_Graph);

		// 41 lines in X direction and 41 lines in Y direction
		for (int i = 0; i < 41 * 2; i++)
		{
			glDrawArrays(GL_LINES, i * 2, 2);
		}
		glBindVertexArray(0);
		//------------------ End of graph----------------------------------------------------

		//---------------------- Draw Axises------------------------------------------------
		glBindVertexArray(gVao_Axis);
			// X- Axis
			glDrawArrays(GL_LINES, 0, 2);
			// Y- Axis
			glDrawArrays(GL_LINES, 2, 2);
		glBindVertexArray(0);
		//--------------------------------- End of axises-------------------------------------

		//--------------------- Draw Rectangle--------------------------------------------------------
		glBindVertexArray(gVao_Rectangle);
		for (int i = 0; i < 8; i++)
		{
			glDrawArrays(GL_LINES, i * 2, 2);
		}
		glBindVertexArray(0);
		//--------------------- End of Rectangle------------------------------------------------------
		
		//--------------------- Circle 1------------------------------------------------------
		glBindVertexArray(gVao_First_Circle);
		for (int i = 0; i < 360 * 9; i++)
		{
			glDrawArrays(GL_POINTS, i, 1);
		}
		glBindVertexArray(0);
		//--------------------- End of Circle 1------------------------------------------------------


		//--------------------- Draw Triangle--------------------------------------------------------
		glBindVertexArray(gVao_Triangle);
		for (int i = 0; i < 6; i++)
		{
			glDrawArrays(GL_LINES, i * 2, 2);
		}
		glBindVertexArray(0);
		//--------------------- End of Triangle------------------------------------------------------

		//-----------------------Incircle-----------------------------------------------------------
		translateMatrix = mat4::identity();
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();

		translateMatrix = vmath::translate(gCenterX, gCenterY, -3.0f);
		modelViewMatrix = translateMatrix;
		modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
		// Circle 1
		glBindVertexArray(gVao_Second_Circle);
		for (int i = 0; i < 360 * 9; i++) {
			glDrawArrays(GL_POINTS, i, 1);
		}
		glBindVertexArray(0);
		//-----------------------End of Incircle-----------------------------------------------------------


	glUseProgram(0);

	// OpenGL Drawing
	SwapBuffers(ghdc);

}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//glOrtho(left, right, bottom, top, near, far);
	gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void UnInitialize()
{
	// UNINITALIZATION CODE
	if(gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	// destroy vao
	if (gVao_Graph)
	{
		glDeleteVertexArrays(1, &gVao_Graph);
		gVao_Graph = 0;
	}

	// detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	// detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	// delete vertex shader object
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;

	// delete fragment shader object
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	// delete shader program object
	glDeleteShader(gShaderProgramObject);
	gShaderProgramObject = 0;

	// delete shader program object
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	// unlink shader program
	glUseProgram(0);

	// Deselect the rendering context
	wglMakeCurrent(NULL, NULL);

	// delete the rendering context
	wglDeleteContext(ghrc);
	ghrc = NULL;

	// Delete the device context
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if(gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
