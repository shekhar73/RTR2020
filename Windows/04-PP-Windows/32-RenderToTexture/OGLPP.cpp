//#define UNICODE
#include <Windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES 1
#include <math.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <time.h>
#include "vmath.h"

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
	SSK_ATTRIBUTE_TEXCORD
};

// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
FILE *gpFile			=	NULL;
HWND ghwnd				= 	NULL;
HDC ghdc				=	NULL;
HGLRC ghrc				= 	NULL;
FILE* gpVendorInfoFile	= NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev	= { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow			= false;
bool gbEscapeKeyIsPressed	= false;
bool gbFullScreen			= false;

GLfloat angleRot = 0.0f;

mat4 perspectiveProjectionMatrix;	

GLuint gShaderProgramObjectColor;
GLuint gShaderProgramObjectTexture;

GLuint vao;
GLuint vbo_position;
GLuint index_buffer;
GLuint fbo;
GLuint color_texture;
GLuint depth_texture;
GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;
GLuint modelMatrixTwoUniform;
GLuint viewMatrixTwoUniform;
GLuint projectionMatrixTwoUniform;
GLuint samplerUniform;


GLuint gVertexShaderObject = 0;
GLuint gVertexShaderLightObject = 0;
GLuint gFragmentShaderObjectColor = 0;
GLuint gFragmentShaderTextureObject = 0;

bool bLight = false;
GLfloat angleCube = 0.0f;

GLuint vao_cube;
GLuint vbo_position_cube;
GLuint vbo_normal_cube;

GLuint ldUniform;
GLuint kdUniform;
GLuint lightPositionUniform;
GLuint lKeyIsPressedUniform;

int ResizeWidth = 0;
int ResizeHeight = 0;

GLfloat currentTime = 0.0f;


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

		case 'L':
		case 'l':
			if (!bLight) {
				bLight = true;
			}
			else {
				bLight = false;
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

	// Light Shader

	gVertexShaderLightObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderLightSourceCode =
	{
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_l_key_is_pressed;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec4 u_light_position;" \
		"out vec3 diffuse_color;" \
		"void main(void)" \
		"{" \
		"if (u_l_key_is_pressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"mat3 normal_matrix = mat3(transpose(inverse(u_view_matrix *u_model_matrix)));" \
		"vec3 t_norm = normalize((normal_matrix) * vNormal);" \
		"vec3 source = normalize(vec3((u_light_position)) - eye_coordinates.xyz);" \
		"diffuse_color = u_ld * u_kd * dot(t_norm, source);" \
		"}" \
		"else" \
		"{"
		"diffuse_color = vec3(1.0, 1.0, 1.0);" \
		"}"
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}"
	};

	GLint iProgramLinkStatus = 0;
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;

	GLchar *szInfoLog = NULL;

	glShaderSource(gVertexShaderLightObject,
		1,
		(const GLchar **)&vertexShaderLightSourceCode,
		NULL);


	glCompileShader(gVertexShaderLightObject);

	// Error Checking

	glGetShaderiv(gVertexShaderLightObject,
		GL_COMPILE_STATUS,
		&iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderLightObject,
			GL_INFO_LOG_LENGTH,
			&iInfoLogLength);

		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(sizeof(GLchar) * iInfoLogLength);

			if (szInfoLog != NULL) {
				GLsizei written;

				glGetShaderInfoLog(gVertexShaderLightObject,
					iInfoLogLength,
					&written,
					szInfoLog);

				fprintf(gpFile, "\n\nVertex Shader Compilation Error: \n%s\n\n", szInfoLog);

				free(szInfoLog);
				UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////
	//*** FRAGMENT SHADER ***
	///////////////////////////////////////////////////////////////////////////////////////

	// Create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCode =
	{
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \
		"out vec4 out_color;" \
		"out vec2 out_texcoord;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"out_color = vPosition * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);" \
		"out_texcoord = vTexCoord;" \
		"}"
	};

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	glCompileShader(gVertexShaderObject);

	// fragment shader compilation error checking

	
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(sizeof(GLchar) * iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "\n\nVertex Shader Compilation Error: \n%s\n\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}


	// Create shader
	
	gFragmentShaderObjectColor = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCodeOne =
	{
		"#version 450 core" \
		"\n" \
		"in vec3 diffuse_color;" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(diffuse_color, 1.0);" \
		"}"
	};

	glShaderSource(gFragmentShaderObjectColor, 1, (const GLchar **)&fragmentShaderSourceCodeOne, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObjectColor);

	// fragment shader compilation error checking

	
	glGetShaderiv(gFragmentShaderObjectColor, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragmentShaderObjectColor, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(sizeof(GLchar) * iInfoLogLength);

			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectColor, iInfoLogLength, &written, szInfoLog);

				fprintf(gpFile, "\n\nFragment Shader Compilation Error: \n%s\n\n", szInfoLog);

				free(szInfoLog);
				UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//*** SHADER PROGRAM ***
	/////////////////////////////////////////////////////////////////////////////////////

	// create
	gShaderProgramObjectColor = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectColor, gVertexShaderLightObject);


	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectColor, gFragmentShaderObjectColor);

	// pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
	glBindAttribLocation(gShaderProgramObjectColor, SSK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectColor, SSK_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObjectColor);

	// shader link error checking
	glGetProgramiv(gShaderProgramObjectColor, GL_LINK_STATUS, &iProgramLinkStatus);

	if (iProgramLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObjectColor, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(sizeof(GLchar) * iInfoLogLength);

			if (szInfoLog != NULL) {
				GLsizei written;

				glGetProgramInfoLog(gShaderProgramObjectColor, iInfoLogLength, &written, szInfoLog);

				fprintf(gpFile, "\n\nProgram Linking Error: \n%s\n\n", szInfoLog);

				free(szInfoLog);
				UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}

	}

	// get MVP uniform location
	// post linking
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_model_matrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_view_matrix");
	projectionMatrixUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_projection_matrix");
	ldUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_ld");
	kdUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_kd");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_light_position");
	lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObjectColor, "u_l_key_is_pressed");
	///--------------------------------------------------------------------------------------------------------------
	
	gFragmentShaderTextureObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderTextureSourceCode =
	{
		"#version 450 core" \
		"\n" \
		"in vec4 out_color;" \
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_sampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = mix(out_color, texture(u_sampler, out_texcoord), 0.7);" \
		"}"
	};

	glShaderSource(gFragmentShaderTextureObject, 1, (const GLchar **)&fragmentShaderTextureSourceCode, NULL);

	glCompileShader(gFragmentShaderTextureObject);

	glGetShaderiv(gFragmentShaderTextureObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragmentShaderTextureObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(sizeof(GLchar) * iInfoLogLength);

			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderTextureObject, iInfoLogLength, &written, szInfoLog);

				fprintf(gpFile, "\n\nFragment Shader Compilation Error: \n%s\n\n", szInfoLog);

				free(szInfoLog);
				UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	gShaderProgramObjectTexture = glCreateProgram();
	
	glAttachShader(gShaderProgramObjectTexture, gVertexShaderObject);
	glAttachShader(gShaderProgramObjectTexture, gFragmentShaderTextureObject);

	glBindAttribLocation(gShaderProgramObjectTexture, SSK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectTexture, SSK_ATTRIBUTE_TEXCORD, "vTexCoord");

	glLinkProgram(gShaderProgramObjectTexture);

	glGetProgramiv(gShaderProgramObjectTexture, GL_LINK_STATUS, &iProgramLinkStatus);

	if (iProgramLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObjectTexture, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(sizeof(GLchar) * iInfoLogLength);

			if (szInfoLog != NULL) {
				GLsizei written;

				glGetProgramInfoLog(gShaderProgramObjectTexture, iInfoLogLength, &written, szInfoLog);

				fprintf(gpFile, "\n\nProgram Linking Error: \n%s\n\n", szInfoLog);

				free(szInfoLog);
				UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	modelMatrixTwoUniform = glGetUniformLocation(gShaderProgramObjectTexture, "u_model_matrix");
	viewMatrixTwoUniform = glGetUniformLocation(gShaderProgramObjectTexture, "u_view_matrix");
	projectionMatrixTwoUniform = glGetUniformLocation(gShaderProgramObjectTexture, "u_projection_matrix");
	samplerUniform = glGetUniformLocation(gShaderProgramObjectTexture, "u_sampler");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLushort vertex_indices[] = 
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	static const GLfloat vertex_data[] =
	{		// Position              Tex Coord
		-0.25f, -0.25f, 0.25f,		0.0f, 1.0f,
		-0.25f, -0.25f, -0.25f,		0.0f, 0.0f,
		0.25f, -0.25f, -0.25f,		1.0f, 0.0f,

		0.25f, -0.25f, -0.25f,		1.0f, 0.0f,
		0.25f, -0.25f, 0.25f,		1.0f, 1.0f,
		-0.25f, -0.25f, 0.25f,		0.0f, 1.0f,

		0.25f, -0.25f, -0.25f,		0.0f, 0.0f,
		0.25f, 0.25f, -0.25f,		1.0f, 0.0f,
		0.25f, -0.25f, 0.25f,		0.0f, 1.0f,

		0.25f, 0.25f, -0.25f,		1.0f, 0.0f,
		0.25f, 0.25f, 0.25f,		1.0f, 1.0f,
		0.25f, -0.25f, 0.25f,		0.0f, 1.0f,

		0.25f, 0.25f, -0.25f,		1.0f, 0.0f,
		-0.25f, 0.25f, -0.25f,		0.0f, 0.0f,
		0.25f, 0.25f, 0.25f,		1.0f, 1.0f,

		-0.25f, 0.25f, -0.25f,		0.0f, 0.0f,
		-0.25f, 0.25f, 0.25f,		0.0f, 1.0f,
		0.25f, 0.25f, 0.25f,		1.0f, 1.0f,

		-0.25f, 0.25f, -0.25f,		1.0f, 0.0f,
		-0.25f, -0.25f, -0.25f,		0.0f, 0.0f,
		-0.25f, 0.25f, 0.25f,		1.0f, 1.0f,

		-0.25f, -0.25f, -0.25f,		0.0f, 0.0f,
		-0.25f, -0.25f, 0.25f,		0.0f, 1.0f,
		-0.25f, 0.25f, 0.25f,		1.0f, 1.0f,

		-0.25f, 0.25f, -0.25f,		0.0f, 1.0f,
		0.25f, 0.25f, -0.25f,		1.0f, 1.0f,
		0.25f, -0.25f, -0.25f,		1.0f, 0.0f,

		0.25f, -0.25f, -0.25f,		1.0f, 0.0f,
		-0.25f, -0.25f, -0.25f,		0.0f, 0.0f,
		-0.25f, 0.25f, -0.25f,		0.0f, 1.0f,

		-0.25f, -0.25f, 0.25f,		0.0f, 0.0f,
		0.25f, -0.25f, 0.25f,		1.0f, 0.0f,
		0.25f, 0.25f, 0.25f,		1.0f, 1.0f,

		0.25f, 0.25f, 0.25f,		1.0f, 1.0f,
		-0.25f, 0.25f, 0.25f,		0.0f, 1.0f,
		-0.25f, -0.25f, 0.25f,		0.0f, 0.0f,
	};

	// Vertices
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);

	glVertexAttribPointer(SSK_ATTRIBUTE_TEXCORD, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXCORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Indices
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Cube
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	const GLfloat cube_vertex_data[] =
	{
		// Front Face
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// Back Face
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		// Right Face
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		// Left Face
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

		// Top Face
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// Bottom Face
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	const GLfloat normal_data[] =
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex_data), cube_vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Normals
	glGenBuffers(1, &vbo_normal_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data), normal_data, GL_STATIC_DRAW);

	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Frame buffer
	glGenFramebuffers(1, &fbo);
	//glCreateFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &color_texture);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 1536, 1536);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_DEPTH_COMPONENT32F, 1536, 1536);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	perspectiveProjectionMatrix = mat4::identity();		

	// Warm up call
	Resize(WIN_WIDTH, WIN_HEIGHT);

}


void Resize(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//glOrtho(left, right, bottom, top, near, far);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(gShaderProgramObjectColor);


	static const GLfloat Green[] = { 0.0f, 0.4f, 0.0f, 1.0f };
	static const GLfloat blue[] = { 0.0f, 0.0f, 0.3f, 1.0f };
	static const GLfloat one = 1.0f;
	currentTime += 0.01f;
	GLfloat f = (GLfloat)currentTime * 0.3f;

	// OpenGL Drawing
	// set modelview & modelviewprojection matrices to identity
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	
	projectionMatrix = perspectiveProjectionMatrix;

	modelMatrix = translate(0.0f, 0.0f, -4.0f) *
		translate(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
		scale(2.5f, 2.5f, 2.5f) *
		rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
		rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);


	glViewport(0, 0, 1536, 1536);
	glClearBufferfv(GL_COLOR, 0, Green);
	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);	

	glUseProgram(gShaderProgramObjectColor);

	glBindVertexArray(vao_cube);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	modelMatrix = translate(0.0f, 0.0f, -5.0f) * rotate(angleCube, angleCube, angleCube);// *scale(0.65f, 0.65f, 0.65f);
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	if (bLight) {
		glUniform1i(lKeyIsPressedUniform, 1);
		glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);
		glUniform4f(lightPositionUniform, 0.0f, 0.0f, -2.0f, 1.0f);
	}
	else {
		glUniform1i(lKeyIsPressedUniform, 0);
	}

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glViewport(0, 0, ResizeWidth, ResizeHeight);
	glClearBufferfv(GL_COLOR, 0, blue);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture);

	modelMatrix = mat4::identity();
	modelMatrix = translate(0.0f, 0.0f, -0.5f) * scale(0.25f, 0.25f, 0.25f) * rotate(45.0f, 45.0f, 45.0f);

	glUseProgram(gShaderProgramObjectTexture);

	glBindVertexArray(vao);

	glUniform1i(samplerUniform, 0);
	glUniformMatrix4fv(viewMatrixTwoUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(modelMatrixTwoUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(projectionMatrixTwoUniform, 1, GL_FALSE, projectionMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);

}



void UnInitialize()
{
	if (vbo_position) {
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (index_buffer) {
		glDeleteBuffers(1, &index_buffer);
		index_buffer = 0;
	}

	if (vao) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (gShaderProgramObjectColor) {
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(gShaderProgramObjectColor);

		glGetProgramiv(gShaderProgramObjectColor,
			GL_ATTACHED_SHADERS,				
			&shaderCount);						

		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		if (pShaders) {
			glGetAttachedShaders(gShaderProgramObjectColor,
				shaderCount,								
				&shaderCount,								
				pShaders);									

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(gShaderProgramObjectColor, pShaders[shaderNumber]);

				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}

			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObjectColor);
		gShaderProgramObjectColor = 0;

		glUseProgram(0);
	}


	if (gbFullScreen == true) {
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ghrc) {
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpFile) {
		fprintf(gpFile, "Log File Closed Successfully.");
		fclose(gpFile);
		gpFile = NULL;
	}
}
