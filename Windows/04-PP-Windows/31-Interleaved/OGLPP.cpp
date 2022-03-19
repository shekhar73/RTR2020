#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "MyWindow.h"
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
	SSK_ATTRIBUTE_TEXTURE0
};

// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
FILE *gpFile			=	NULL;
HWND ghwnd				= 	NULL;
HDC ghdc				=	NULL;
HGLRC ghrc				= 	NULL;
FILE* gpVendorInfoFile	=   NULL;

bool bLight =   false;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev	= { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow			= false;
bool gbEscapeKeyIsPressed	= false;
bool gbFullScreen			= false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint marbleTexture;
GLuint textureSamplerUniform;

GLuint gVaoCube;
GLuint gVboPCNTCube;

GLuint modelViewMatrixUniform;
GLuint perspectiveProjectionUniform;
GLuint LKeyPressedUniform;
GLuint LdUniform;
GLuint KdUniform;
GLuint lightPositionUniform;

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
			DestroyWindow(hwnd);
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			if (bLight == false)
				bLight = true;
			else
				bLight = false;
			break;
		}
		break;

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CLOSE:
		UnInitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		UnInitialize();
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
	bool LoadGLTexture(GLuint*, TCHAR[]);
	
	// Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ghdc = GetDC(ghwnd);
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// Initialization of structure 'PIXELFORMATDESCRIPTO'
	// Parallel to glutInitDisplayMode()
	pfd.nSize 	 	= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags 	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType  = PFD_TYPE_RGBA;
	pfd.cColorBits 	= 32;
	pfd.cRedBits 	= 8;
	pfd.cGreenBits 	= 8;
	pfd.cBlueBits 	= 8;
	pfd.cAlphaBits 	= 8;
	pfd.cDepthBits 	= 32;

	ghdc = GetDC(ghwnd);

	// choos a pixel format which best matches with that of 'pfd'
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// set the pixel format chosen above
	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// make the rendering above us current n the current hdc
	if(wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// GLEW Initialization code for GLSL (IMPORTANT : It must be here. 
	//Means after creating OpenGL context but before using any OpengGL function)
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
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
	const GLchar *vertexShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;"	\
		"in vec2 vTexCoord;"	\
		"in vec3 vNormal;"	\
		"uniform mat4 u_model_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_LKeyPressed;"	\
		"uniform vec3 u_ld;"	\
		"uniform vec3 u_kd;"	\
		"uniform vec4 u_light_position;"	\
		"out vec3 diffuse_light;"	\
		"out vec2 out_TexCoord;"	\
		"out vec4 out_color;"	\
		"void main(void)" \
		"\n" \
		"{" \
		"if(u_LKeyPressed == 1)"	\
		"{"	\
			"vec4 eye_coordinates = u_model_view_matrix * vPosition;"	\
			"mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));"	\
			"vec3 tnorm = normalize(normal_matrix * vNormal);"	\
			"vec3 s = normalize(vec3(u_light_position - eye_coordinates));"	\
			"diffuse_light = u_ld * u_kd * max(dot(s, tnorm), 0.0f);"	\
		"}"	\
		"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;" \
		"out_TexCoord = vTexCoord;"	\
		"out_color = vColor;"	\
		"}";

	//gl_Position is inbuilt variable in shader
	// feed the source code to your shaderobject
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject);
	
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	// shader compilation error checking

	// void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	// glGetShaderiv — return a parameter from a shader object

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
	const GLchar *fragmentShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec3 diffuse_light;"	\
		"uniform int u_LKeyPressed;"	\
		"in vec2 out_TexCoord;"	\
		"in vec4 out_Color;"	\
		"uniform sampler2D u_texture_sampler;"	\
		"out vec4 FragColor;"	\
		"void main(void)" \
		"\n" \
		"{" \
			"vec4 tex = vec4(texture(u_texture_sampler, out_TexCoord));"	\
			"vec4 light;"	\
			"if(u_LKeyPressed == 1)"	\
			"{"	\
				"light = vec4(diffuse_light, 1.0f);"	\
			"}"	\
			"else"	\
			"{"
				"light = vec4(1.0f, 1.0f, 1.0f, 1.0f);"	\
			"}"	\
			"FragColor = tex * light * out_Color;" \
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

	// pre-link binding of shader program object with vertex shader position attribute in your enum to catch the 'in' and 'uniform' attributes
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXTURE0, "vTexCoord");

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
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
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
	textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
	modelViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_view_matrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	const GLfloat PCNT_cube[] = {
				 0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
				-0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
				-0.5f,-0.5f, 0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
				 0.5f,-0.5f, 0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,

				 0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
				 0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
				 0.5f,-0.5f, 0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
				 0.5f,-0.5f,-0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

				-0.5f, 0.5f,-0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
				 0.5f, 0.5f,-0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
				 0.5f,-0.5f,-0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
				-0.5f,-0.5f,-0.5f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

				-0.5f, 0.5f, 0.5f,   1.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
				-0.5f, 0.5f,-0.5f,   1.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
				-0.5f,-0.5f,-0.5f,   1.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
				-0.5f,-0.5f, 0.5f,   1.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

				 0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
				-0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
				-0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
				 0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

				 0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
				-0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
				-0.5f,-0.5f, 0.5f,  1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
				 0.5f,-0.5f, 0.5f,  1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f
			};

	// It will store all the below 6 steps so that we can reuse it in draw function
	glGenVertexArrays(1, &gVaoCube);
	glBindVertexArray(gVaoCube);

	glGenBuffers(1, &gVboPCNTCube);	//like glGenTextures() in FFP

	glBindBuffer(GL_ARRAY_BUFFER, gVboPCNTCube);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, 24 * 11 * sizeof(float), PCNT_cube, GL_STATIC_DRAW);

	// POSITION
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);

	// COLOR
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);

	// NORMAL
	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);

	// TEXTURE
	glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
	glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);

	// Unbind the BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	// set-up detpth buffer
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// depth test to do
	glDepthFunc(GL_LEQUAL);
	// set really nice perspective calculations?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	LoadGLTexture(&marbleTexture, MAKEINTRESOURCE(MARBLE_BITMAP));

	// we will always cull back faces for better performance
	glEnable(GL_TEXTURE_2D);

	// set background color to which it will display even if it will empty.
	// THIS LINE CAN BE IN drawRect().
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// blue

	// set PerspectiveProjectionMatrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	// resize
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

bool LoadGLTexture(GLuint* Texture, TCHAR resourceID[])
{
	//Variable Declartions
	bool bResult = false;
	HBITMAP hBitmap = NULL;    // OS Image Lading
	BITMAP bmp;                // OS Image Lading

	//code  :  Real Texture Code : Very Important

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		resourceID,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION); // KARAN he function handel return karat, : GetModuleHandel Hinstance deto...

	if (hBitmap)
	{
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);   // Ithe Image loading code sampla

		//From Here Start OpenGl code

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, Texture);   // GPU side la ek target pointer tayar zala; ani aplyala gattu milala : Ithe Address aahe
		glBindTexture(GL_TEXTURE_2D, *Texture); // Ithe Value aahe

		//Setting of Texture Param
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Atta Data Tkaycha ahe Graphics card side la with the help of Graphic driver
		//gluBuild2DMipmaps(GL_TEXTURE_2D,3,bmp.bmWidth,bmp.bmHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);  // This is OS Fuction  : AApan texture use kela pn nahiye tari delete kela karan to load zalay...
	}

	return bResult;
}


void Resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//glOrtho(left, right, bottom, top, near, far);
	gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}


void Display(void)
{
	static GLfloat angleCube = 0.0f;

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f);
		GLfloat lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
		glUniform4fv(lightPositionUniform, 1, lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	// OpenGL Drawing
	// set modelview & modelviewprojection matrices to identity
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();
	mat4 translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();
	mat4 rotationMatrix_X = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();
	mat4 rotationMatrix_Y = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();
	mat4 rotationMatrix_Z = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();

	rotationMatrix_X = vmath::rotate((GLfloat)angleCube, 1.0f, 0.0f, 0.0f);
	rotationMatrix_Y = vmath::rotate((GLfloat)angleCube, 0.0f, 1.0f, 0.0f);
	rotationMatrix_Z = vmath::rotate((GLfloat)angleCube, 0.0f, 0.0f, 1.0f);

	// multiply the modelview and perspective matrix to get modelviewprojection matrix
	modelViewMatrix = translateMatrix * rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;	// ORDER IS IMPORTANT because Matrix mulitplication is not commutative i.e. (a * b) != (b * a)
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
	
	glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marbleTexture);
	glUniform1i(textureSamplerUniform, 0);

	// *** bind vao ***
	// all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVaoCube);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 (each with its x, y, z) vertices in triangleVertices array
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);


	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	angleCube = angleCube + 0.1f;
	if (angleCube >= 360.0f)
	{
		angleCube = 0.0f;
	}

	// OpenGL Drawing
	SwapBuffers(ghdc);

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

	// destory vao
	if (gVaoCube)
	{
		glDeleteVertexArrays(1, &gVaoCube);
		gVaoCube = 0;
	}

	// destroy vbo
	if (gVboPCNTCube)
	{
		glDeleteVertexArrays(1, &gVboPCNTCube);
		gVboPCNTCube = 0;
	}


	if (gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);
		GLsizei shaderCount;

		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;

		pShaders = (GLuint*)malloc(shaderCount * sizeof(GLuint));
		if (pShaders == NULL)
		{
			printf("Malloc Failed!!!Exitting Now!!\n\n");
			exit(0);
		}

		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
			free(pShaders);

			glDeleteProgram(gShaderProgramObject);
			gShaderProgramObject = 0;
			glUseProgram(0);
		}
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

	if(gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
