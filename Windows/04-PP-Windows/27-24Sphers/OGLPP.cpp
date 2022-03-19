#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/GL.h>

#include "vmath.h"	// It is a C++ file
#include "Sphere.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Sphere.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

// we will give these values to the VERTEX which have position, color, normal, texture,
// velocity, gravity, viscocity, shineness, damping, blurring...more properties
enum
{
	SSK_ATTRIBUTE_POSITION = 0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0
};

// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
FILE* gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE* gpVendorInfoFile = NULL;

int keyPressed_1;
int keyPressed_2;
int keyPressed_3;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;
bool bLight;
bool FragmentShader = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint gVao_sphere;
GLuint gNumVertices;
GLuint gNumElements;

GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLfloat factor = 10.0f;
GLfloat angle_X = 0.0f;
GLfloat angle_Y = 0.0f;
GLfloat angle_Z = 0.0f;

GLfloat materialAmbient[4];
GLfloat materialDiffused[4];
GLfloat materialSpecular[4];
GLfloat materialShininess;

GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };

GLuint gModelMatrixUniform;
GLuint gViewMatrixUniform;
GLuint gPerspectiveProjectionMatrixUniform;

// Light
GLuint LaUniform;
GLuint LdUniform;
GLuint LsUniform;
GLuint lightPositionUniform;

// Material
GLuint KaUniform;
GLuint KdUniform;
GLuint KsUniform;
GLfloat KshineUniform;

GLuint LKeyPressedUniform;

// It is a matrix of 4 X 4 which is declared in vmath.h header file. It will be used for transformation
mat4 orthographicProjectionMatrix;


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

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable Pipeline : Sphere"),
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
			Display();

			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
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
		if (HIWORD(wParam) == 0)
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
			if (bLight == true)
			{
				bLight = false;
			}
			else
			{
				bLight = true;
			}
			break;


		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{

		case 'X':
		case 'x':
			keyPressed_1 = 1;
			angle_X = 0.0f;
			break;

		case 'Y':
		case 'y':
			keyPressed_2 = 2;
			angle_Y = 0.0f;
			break;

		case 'Z':
		case 'z':
			keyPressed_3 = 3;
			angle_Z = 0.0f;
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
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_perspective_projection_matrix;" \
		"uniform int u_l_key_preesed;" \
		"uniform vec4 u_light_position;" \
		"out vec3 light_direction;" \
		"out vec3 view_vector;" \
		"out vec3 transformed_normal;" \
		"void main(void)" \
		"{" \
			"if(u_l_key_preesed == 1)" \
			"{" \
				"vec4 eye_coordinates		= u_view_matrix * u_model_matrix * vPosition;" \
				"transformed_normal			= mat3(u_view_matrix * u_model_matrix) * vNormal;" \
				"view_vector				= -eye_coordinates.xyz;" \
				"light_direction			= vec3(u_light_position - eye_coordinates);" \
			"}"	\
			"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix  * vPosition;"  \
		"}";

	// eye_coordinates.xyz = swizling
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
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"out vec4 FragColor;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform int u_l_key_preesed;" \
		"in vec3 light_direction;" \
		"in vec3 view_vector;" \
		"in vec3 transformed_normal;" \
		"uniform float u_shineness;" \
		"void main(void)" \
		"{" \
			"vec3 phong_ads_light;"	\
			"if(u_l_key_preesed == 1)" \
			"{" \
				"vec3 normalized_transformed_normals	= normalize(transformed_normal);" \
				"vec3 normalized_light_directions		= normalize(light_direction);" \
				"vec3 normalized_view_vectors			= normalize(view_vector);" \
				"vec3 reflection_vector					= reflect(-normalized_light_directions, normalized_transformed_normals);" \
				"vec3 ambient_light						= u_la * u_ka;" \
				"vec3 diffuse_light						= u_ld * u_kd * max(dot(normalized_light_directions, normalized_transformed_normals), 0.0f);" \
				"vec3 specular 							= u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vectors), 0.0f), u_shineness);" \
				"phong_ads_light						= ambient_light + diffuse_light + specular;" \
			"}"	\
			"else"	\
			"{" \
				"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
			"}" \
			"FragColor = vec4(phong_ads_light, 1.0f);" \
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
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

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


	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	if (gModelMatrixUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	if (gViewMatrixUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gPerspectiveProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
	if (gPerspectiveProjectionMatrixUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
	if (LaUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
	if (LdUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
	if (LsUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	if (KaUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	if (KdUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
	if (KsUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}


	KshineUniform = glGetUniformLocation(gShaderProgramObject, "u_shineness");
	if (materialShininess == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	if (lightPositionUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_l_key_preesed");
	if (LKeyPressedUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}


	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// It will store all the below 6 steps so that we can reuse it in draw function
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// Postion vbo
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind Vao
	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.25f, 0.25f, 0.25f, 0.25f);

	// set PerspectiveProjectionMatrix to identity matrix
	orthographicProjectionMatrix = mat4::identity();

	bLight = false;
	// resize
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	if (width <= height)
	{
		orthographicProjectionMatrix = vmath::ortho(0.0f,
			15.5f,
			0.0f,
			15.5f * ((GLfloat)height / (GLfloat)width),
			-10.0f,
			10.0f);
	}
	else
	{
		orthographicProjectionMatrix = vmath::ortho(0.0f,
			15.5f * ((GLfloat)width / (GLfloat)height),
			0.0f,
			15.5f,
			-10.0f,
			10.0f);
	}
}


void Display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// || 1 Sphere|| Emerald ----------------------------------------------------------------------
	// start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0215;
	materialAmbient[1] = 0.1745;
	materialAmbient[2] = 0.0215;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.07568;
	materialDiffused[1] = 0.61424;
	materialDiffused[2] = 0.07568;
	materialDi0.ffused[3] = 1.0f;

	materialSpecular[0] = 0.633;
	materialSpecular[1] = 0.727811;
	materialSpecular[2] = 0.633;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.6f * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 projectionMatrix = mat4::identity();
	mat4 translateMatrix = vmath::translate(1.5f, 14.0f, 0.0f);
	mat4 scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// Jade || 2 Sphere||----------------------------------------------------------------------
	
	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.135f;
	materialAmbient[1] = 0.2225f;
	materialAmbient[2] = 0.1575f;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.54f;
	materialDiffused[1] = 0.89f;
	materialDiffused[2] = 0.63f;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.316228f;
	materialSpecular[1] = 0.316228f;
	materialSpecular[2] = 0.316228f;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.1f * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(26.0f, 14.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// || obsidian 3 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.05375;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.06625;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.18275;
	materialDiffused[1] = 0.17;
	materialDiffused[2] = 0.22525;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.332741;
	materialSpecular[1] = 0.328634;
	materialSpecular[2] = 0.346435;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.3 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(11.5f, 14.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// || pearl 4 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.25;
	materialAmbient[1] = 0.20725;
	materialAmbient[2] = 0.20725;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 1.0;
	materialDiffused[1] = 0.829;
	materialDiffused[2] = 0.829;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.296648;
	materialSpecular[1] = 0.296648;
	materialSpecular[2] = 0.296648;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.088 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(6.5f, 14.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// || ruby 5 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.1745;
	materialAmbient[1] = 0.01175;
	materialAmbient[2] = 0.01175;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.61424;
	materialDiffused[1] = 0.04136;
	materialDiffused[2] = 0.04136;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.727811;
	materialSpecular[1] = 0.626959;
	materialSpecular[2] = 0.626959;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.6 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(16.5f, 14.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	

	// || turquoise 6 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.1;
	materialAmbient[1] = 0.18725;
	materialAmbient[2] = 0.1745;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.396;
	materialDiffused[1] = 0.74151;
	materialDiffused[2] = 0.69102;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.297254;
	materialSpecular[1] = 0.30829;
	materialSpecular[2] = 0.306678;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.1 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(21.5f, 14.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// ||brass 7 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.329412;
	materialAmbient[1] = 0.223529;
	materialAmbient[2] = 0.027451;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.780392;
	materialDiffused[1] = 0.568627;
	materialDiffused[2] = 0.113725;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.992157;
	materialSpecular[1] = 0.941176;
	materialSpecular[2] = 0.807843;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.21794872 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(1.5f, 10.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// || bronze 8 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.2125;
	materialAmbient[1] = 0.1275;
	materialAmbient[2] = 0.054;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.714;
	materialDiffused[1] = 0.4284;
	materialDiffused[2] = 0.18144;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.393548;
	materialSpecular[1] = 0.271906;
	materialSpecular[2] = 0.166721;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.2 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(21.5f, 14.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || chrome 9 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.25;
	materialAmbient[1] = 0.25;
	materialAmbient[2] = 0.25;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.4;
	materialDiffused[1] = 0.4;
	materialDiffused[2] = 0.4;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.774597;
	materialSpecular[1] = 0.774597;
	materialSpecular[2] = 0.774597;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.6 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(6.5f, 10.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || copper 10 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.19125;
	materialAmbient[1] = 0.0735;
	materialAmbient[2] = 0.0225;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.7038;
	materialDiffused[1] = 0.27048;
	materialDiffused[2] = 0.0828;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.256777;
	materialSpecular[1] = 0.137622;
	materialSpecular[2] = 0.086014;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.1 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(11.5f, 10.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || gold 11 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.24725;
	materialAmbient[1] = 0.1995;
	materialAmbient[2] = 0.0745;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.75164;
	materialDiffused[1] = 0.60648;
	materialDiffused[2] = 0.22648;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.628281;
	materialSpecular[1] = 0.555802;
	materialSpecular[2] = 0.366065;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.4 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(16.5f, 10.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// ||silver 12 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.19225;
	materialAmbient[1] = 0.19225;
	materialAmbient[2] = 0.19225;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.50754;
	materialDiffused[1] = 0.50754;
	materialDiffused[2] = 0.50754;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.508273;
	materialSpecular[1] = 0.508273;
	materialSpecular[2] = 0.508273;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.4 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(21.5f, 10.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// 3rd Coloumn
	//////////////////////////////////////////////////////////
	// black  13
	//////////////////////////////////////////////////////////

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.01;
	materialDiffused[1] = 0.01;
	materialDiffused[2] = 0.01;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.50;
	materialSpecular[1] = 0.50;
	materialSpecular[2] = 0.50;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.25 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(26.0f, 10.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || cyan 14 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.1;
	materialAmbient[2] = 0.06;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.0;
	materialDiffused[1] = 0.50980392;
	materialDiffused[2] = 0.50980392;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.50196078;
	materialSpecular[1] = 0.50196078;
	materialSpecular[2] = 0.50196078;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.25 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(1.5f, 6.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || green 15 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.1;
	materialDiffused[1] = 0.35;
	materialDiffused[2] = 0.1;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.45;
	materialSpecular[1] = 0.55;
	materialSpecular[2] = 0.45;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.25 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(6.5f, 6.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || red 16 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.5;
	materialDiffused[1] = 0.0;
	materialDiffused[2] = 0.0;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.6;
	materialSpecular[2] = 0.6;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.25 * 128;


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(11.5f, 6.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || white 17 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.55;
	materialDiffused[1] = 0.55;
	materialDiffused[2] = 0.55;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.70;
	materialSpecular[1] = 0.70;
	materialSpecular[2] = 0.70;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.25 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(16.5f, 6.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || yellow plastic 18 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.5;
	materialDiffused[1] = 0.5;
	materialDiffused[2] = 0.0;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.60;
	materialSpecular[1] = 0.60;
	materialSpecular[2] = 0.50;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.25 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(21.5f, 6.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	/// 4th coloumn
	//////////////////////////////////////////////////////////
	// black 19
	//////////////////////////////////////////////////////////

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.02;
	materialAmbient[1] = 0.02;
	materialAmbient[2] = 0.02;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.01;
	materialDiffused[1] = 0.01;
	materialDiffused[2] = 0.01;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.4;
	materialSpecular[1] = 0.4;
	materialSpecular[2] = 0.4;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(26.0f, 6.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);
	

	// || cyan 20 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.05;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.4;
	materialDiffused[1] = 0.5;
	materialDiffused[2] = 0.5;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.04;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.7;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(1.5f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);
	

	// || green 21 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.4;
	materialDiffused[1] = 0.5;
	materialDiffused[2] = 0.4;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.04;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(6.5f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || red 22 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.05;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.5;
	materialDiffused[1] = 0.4;
	materialDiffused[2] = 0.4;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.04;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(11.5f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);


	// || white 23 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.05;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.05;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.5;
	materialDiffused[1] = 0.5;
	materialDiffused[2] = 0.5;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.7;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(16.5f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// ||  yellow rubber  24 Sphere||----------------------------------------------------------------------

	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.05;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.5;
	materialDiffused[1] = 0.5;
	materialDiffused[2] = 0.4;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(21.5f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// 25
	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.15;
	materialAmbient[1] = 0.15;
	materialAmbient[2] = 0.01;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.5;
	materialDiffused[1] = 0.25;
	materialDiffused[2] = 0.14;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.27;
	materialSpecular[1] = 0.37;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(26.0f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// 26
	glUseProgram(gShaderProgramObject);

	if (bLight == true)
	{
		if (keyPressed_1 == 1)
		{
			angle_X = angle_X + 0.001f;
			lightPosition[0] = 0.0f;
			lightPosition[1] = factor * sin(angle_X);
			lightPosition[2] = factor * cos(angle_X);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_2 == 2)
		{
			angle_Y = angle_Y + 0.001f;
			lightPosition[0] = factor * sin(angle_Y);
			lightPosition[1] = 0.0f;
			lightPosition[2] = factor * cos(angle_Y);
			lightPosition[3] = 1.0f;
		}
		else if (keyPressed_3 == 3)
		{
			angle_Z = angle_Z + 0.001f;
			lightPosition[0] = factor * sin(angle_Z);
			lightPosition[1] = factor * cos(angle_Z);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform1f(KshineUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffused);
		glUniform3fv(KsUniform, 1, materialSpecular);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	glBindVertexArray(gVao_sphere);

	materialAmbient[0] = 0.25;
	materialAmbient[1] = 0.35;
	materialAmbient[2] = 0.41;
	materialAmbient[3] = 1.0f;

	materialDiffused[0] = 0.25;
	materialDiffused[1] = 0.35;
	materialDiffused[2] = 0.44;
	materialDiffused[3] = 1.0f;

	materialSpecular[0] = 0.57;
	materialSpecular[1] = 0.47;
	materialSpecular[2] = 0.34;
	materialSpecular[3] = 1.0f;

	materialShininess = 0.078125 * 128;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	translateMatrix = vmath::translate(1.5f, 2.0f, 0.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = orthographicProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	if (angle_X >= 360.0f)
	{
		angle_X = 0.0f;
	}

	if (angle_Y >= 360.0f)
	{
		angle_Y = 0.0f;
	}

	if (angle_Z >= 360.0f)
	{
		angle_Z = 0.0f;
	}
	// OpenGL Drawing
	SwapBuffers(ghdc);

}

void UnInitialize()
{
	// UNINITALIZATION CODE
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	// destory vao
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	// destroy vbo
	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
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

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
