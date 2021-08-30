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

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;
bool gbVKeyPressed = true;
bool gbFKeyPressed = false;


// Per Vertex Shader
GLuint gVertexShaderObject_PV;
GLuint gFragmentShaderObject_PV;
GLuint gShaderProgramObject_PV;

//////////////////////////////////////
// PER VERTEX UNIFORMS
///////////////////////////////////////
GLuint gModelMatrix_PV;
GLuint gViewMatrix_PV;
GLuint gProjectionMatrix_PV;

// Per Vertex Light
GLuint LaUniform_PV;
GLuint LdUniform_PV;
GLuint LsUniform_PV;

// Per Vertex Material
GLuint KaUniform_PV;
GLuint KdUniform_PV;
GLuint KsUniform_PV;

GLfloat materialShineness_PV;
GLuint lightPositionUniform_PV;


// Per Fragment Shader
GLuint gVertexShaderObject_PF;
GLuint gFragmentShaderObject_PF;
GLuint gShaderProgramObject_PF;

//////////////////////////////////////
// PER FRAGMENT UNIFORMS
///////////////////////////////////////
GLuint gModelMatrix_PF;
GLuint gViewMatrix_PF;
GLuint gProjectionMatrix_PF;

// Per Vertex Light
GLuint LaUniform_PF;
GLuint LdUniform_PF;
GLuint LsUniform_PF;

// Per Vertex Material
GLuint KaUniform_PF;
GLuint KdUniform_PF;
GLuint KsUniform_PF;

GLfloat materialShineness_PF;
GLuint lightPositionUniform_PF;

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint chooseShader = 1;
GLuint gNumVertices;
GLuint gNumElements;

GLuint LKeyPressedUniform;


bool bLight;



// It is a matrix of 4 X 4 which is declared in vmath.h header file. It will be used for transformation
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
				gbEscapeKeyIsPressed = true;
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

		case 49:
			chooseShader = 1;
			break;

		case 50:
			chooseShader = 2;
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
			if (bLight == true)
			{
				bLight = false;
			}
			else
			{
				bLight = true;
			}
			break;

		case 'E':
		case 'e':
			if (gbEscapeKeyIsPressed == false)
			{
				gbEscapeKeyIsPressed = true;
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
	//***PER VERTEX SHADER ***
	//////////////////////////////////////////////////////////////////////////////////////

	// Create Shader
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	// Provide source code to shader
	const GLchar* vertexShaderSourceCode_PV =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform int u_l_key_preesed;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ks;" \
		"uniform vec4 u_light_position;" \
		"uniform float u_shineness;" \
		"out vec3 fong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_l_key_preesed == 1)" \
		"{" \
		"vec4 eye_coordinates		= u_viewMatrix * u_modelMatrix * vPosition;" \
		"vec3 transformed_normal	= normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
		"vec3 light_direction		= normalize(vec3(u_light_position - eye_coordinates));" \
		"vec3 reflection_vector		= reflect(-light_direction, transformed_normal);" \
		"vec3 view_vector			= normalize(-eye_coordinates.xyz);" \
		"vec3 ambient_light			= u_la * u_ka;" \
		"vec3 diffuse_light			= u_ld * u_kd * max(dot(light_direction, transformed_normal), 0.0f);" \
		"vec3 specular 				= u_ls * u_ks * pow(max(dot(reflection_vector, view_vector), 0.0f), u_shineness);" \
		"fong_ads_light			    = ambient_light + diffuse_light + specular;" \
		"}"	\
		"else"	\
		"{" \
		"fong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix  * vPosition;"  \
		"}";

	// eye_coordinates.xyz = swizling
	//gl_Position is inbuilt variable in shader
	// feed the source code to your shaderobject
	glShaderSource(gVertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject_PV);

	GLint iInfoLogLength_PV = 0;
	GLint iShaderCompiledStatus_PV = 0;
	char* szInfoLog_PV = NULL;

	// shader compilation error checking

	//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	// glGetShaderiv — return a parameter from a shader object

	glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus_PV);
	if (iShaderCompiledStatus_PV == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength_PV);
		if (iInfoLogLength_PV > 0)
		{
			szInfoLog_PV = (char*)malloc(iInfoLogLength_PV);
			if (szInfoLog_PV != NULL)
			{
				GLsizei written_PV;

				// take log
				glGetShaderInfoLog(gVertexShaderObject_PV, iInfoLogLength_PV, &written_PV, szInfoLog_PV);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog_PV);
				free(szInfoLog_PV);
				//destroyWindow(ghwnd);
				UnInitialize(); //destroyWindow(ghwnd);-> call WND_DESTROY->PostQuitMessage()->MessageLoop()->uninitialize();
				exit(0);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//*** PER VERTEX FRAGMENT SHADER ***
	///////////////////////////////////////////////////////////////////////////////////////

	// Create shader
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar* fragmentShaderSourceCode_PV =
		"#version 450 core" \
		"\n" \
		"out vec4 FragColor;" \
		"in vec3 fong_ads_light;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(fong_ads_light, 1.0f);" \
		"}";


	glShaderSource(gFragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject_PV);

	// fragment shader compilation error checking


	glGetShaderiv(gFragmentShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus_PV);
	if (iShaderCompiledStatus_PV == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength_PV);
		if (iInfoLogLength_PV > 0)
		{
			szInfoLog_PV = (char*)malloc(iInfoLogLength_PV);
			if (szInfoLog_PV != NULL)
			{
				GLsizei written_PV;
				glGetShaderInfoLog(gFragmentShaderObject_PV, iInfoLogLength_PV, &written_PV, szInfoLog_PV);
				fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog_PV);
				free(szInfoLog_PV);
				UnInitialize();
				exit(0);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//*** PER VERTEX SHADER PROGRAM ***
	/////////////////////////////////////////////////////////////////////////////////////

	// create
	gShaderProgramObject_PV = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);

	// pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
	glBindAttribLocation(gShaderProgramObject_PV, SSK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject_PV, SSK_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObject_PV);

	GLint iShaderProgramLinkStatus_PV = 0;


	// shader link error checking
	glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderProgramLinkStatus_PV);

	if (iShaderProgramLinkStatus_PV == GL_FALSE)
	{
		glGetShaderiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength_PV);
		if (iInfoLogLength_PV > 0)
		{
			szInfoLog_PV = (char*)malloc(iInfoLogLength_PV);
			if (szInfoLog_PV != NULL)
			{
				GLsizei written_PV;
				glGetShaderInfoLog(gShaderProgramObject_PV, iInfoLogLength_PV, &written_PV, szInfoLog_PV);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog_PV);
				free(szInfoLog_PV);
				UnInitialize();
				exit(0);
			}
		}

	}

	// get MVP uniform location
	// post linking


	gModelMatrix_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_modelMatrix");
	if (gModelMatrix_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gViewMatrix_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_viewMatrix");
	if (gViewMatrix_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gProjectionMatrix_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projectionMatrix");
	if (gProjectionMatrix_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_la");
	if (LaUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ld");
	if (LdUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ls");
	if (LsUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ka");
	if (KaUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_kd");
	if (KdUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ks");
	if (KsUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}


	materialShineness_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_shineness");
	if (materialShineness_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	lightPositionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position");
	if (lightPositionUniform_PV == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_l_key_preesed");
	if (LKeyPressedUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}






















	///////////////////////////////////////////////////////////////////////////////////////
	//***PER FRAGMENT VERTEX SHADER ***
	//////////////////////////////////////////////////////////////////////////////////////


	// Create Shader
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	// Provide source code to shader
	const GLchar* vertexShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform int u_l_key_preesed;" \
		"uniform vec4 u_light_position;" \
		"out vec3 light_direction;" \
		"out vec3 view_vector;" \
		"out vec3 transformed_normal;" \
		"void main(void)" \
		"{" \
		"if(u_l_key_preesed == 1)" \
		"{" \
		"vec4 eye_coordinates		= u_viewMatrix * u_modelMatrix * vPosition;" \
		"transformed_normal			= mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"light_direction			= vec3(u_light_position - eye_coordinates);" \
		"view_vector				= -eye_coordinates.xyz;" \
		"}"	\
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix  * vPosition;"  \
		"}";

	// eye_coordinates.xyz = swizling
	//gl_Position is inbuilt variable in shader
	// feed the source code to your shaderobject
	glShaderSource(gVertexShaderObject_PF, 1, (const GLchar**)&vertexShaderSourceCode_PF, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject_PF);

	GLint iInfoLogLength_PF = 0;
	GLint iShaderCompiledStatus_PF = 0;
	char* szInfoLog_PF = NULL;

	// shader compilation error checking

	//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	// glGetShaderiv — return a parameter from a shader object

	glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus_PF);
	if (iShaderCompiledStatus_PF == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength_PF);
		if (iInfoLogLength_PF > 0)
		{
			szInfoLog_PF = (char*)malloc(iInfoLogLength_PF);
			if (szInfoLog_PF != NULL)
			{
				GLsizei written;

				// take log
				glGetShaderInfoLog(gVertexShaderObject_PF, iInfoLogLength_PF, &written, szInfoLog_PF);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog_PF);
				free(szInfoLog_PF);
				//destroyWindow(ghwnd);
				UnInitialize(); //destroyWindow(ghwnd);-> call WND_DESTROY->PostQuitMessage()->MessageLoop()->uninitialize();
				exit(0);
			}
		}
	}








	///////////////////////////////////////////////////////////////////////////////////////
	//***PER FRAGMENT FRAGMENT SHADER ***
	//////////////////////////////////////////////////////////////////////////////////////

	// Create shader
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar* fragmentShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"out vec4 FragColor;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ks;" \
		"uniform int u_l_key_preesed;" \
		"in vec3 light_direction;" \
		"in vec3 view_vector;" \
		"in vec3 transformed_normal;" \
		"in vec3 reflection_vector;" \
		"uniform float u_shineness;" \
		"vec3 fong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_l_key_preesed == 1)" \
		"{" \
		"vec3 normalized_transformed_normals	= normalize(transformed_normal);" \
		"vec3 normalized_light_directions		= normalize(light_direction);" \
		"vec3 normalized_view_vectors			= normalize(view_vector);" \
		"vec3 reflection_vector					= reflect(-normalized_light_directions, normalized_transformed_normals);" \
		"vec3 ambient_light						= u_la * u_ka;" \
		"vec3 diffuse_light						= u_ld * u_kd * max(dot(normalized_light_directions, normalized_transformed_normals), 0.0f);" \
		"vec3 specular 							= u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vectors), 0.0f), u_shineness);" \
		"fong_ads_light							= ambient_light + diffuse_light + specular;" \
		"}"	\
		"else"	\
		"{" \
		"fong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"FragColor = vec4(fong_ads_light, 1.0f);" \
		"}";


	glShaderSource(gFragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject_PF);

	// fragment shader compilation error checking


	glGetShaderiv(gFragmentShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus_PF);
	if (iShaderCompiledStatus_PF == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength_PF);
		if (iInfoLogLength_PF > 0)
		{
			szInfoLog_PF = (char*)malloc(iInfoLogLength_PF);
			if (szInfoLog_PF != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PF, iInfoLogLength_PF, &written, szInfoLog_PF);
				fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog_PF);
				free(szInfoLog_PF);
				UnInitialize();
				exit(0);
			}
		}
	}




	

	/////////////////////////////////////////////////////////////////////////////////////
	//*** PER FRAGMENT SHADER PROGRAM ***
	/////////////////////////////////////////////////////////////////////////////////////

	// create
	gShaderProgramObject_PF = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);

	// pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
	glBindAttribLocation(gShaderProgramObject_PF, SSK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject_PF, SSK_ATTRIBUTE_NORMAL, "vNormal");

	// link shader
	glLinkProgram(gShaderProgramObject_PF);

	GLint iShaderProgramLinkStatus_PF = 0;

	// shader link error checking
	glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderProgramLinkStatus_PF);

	if (iShaderProgramLinkStatus_PF == GL_FALSE)
	{
		glGetShaderiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength_PF);
		if (iInfoLogLength_PF > 0)
		{
			szInfoLog_PF = (char*)malloc(iInfoLogLength_PF);
			if (szInfoLog_PF != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gShaderProgramObject_PF, iInfoLogLength_PF, &written, szInfoLog_PF);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog_PF);
				free(szInfoLog_PF);
				UnInitialize();
				exit(0);
			}
		}

	}

	// get MVP uniform location
	// post linking


	gModelMatrix_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_modelMatrix");
	if (gModelMatrix_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gViewMatrix_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_viewMatrix");
	if (gViewMatrix_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gProjectionMatrix_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projectionMatrix");
	if (gProjectionMatrix_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_la");
	if (LaUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ld");
	if (LdUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ls");
	if (LsUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ka");
	if (KaUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_kd");
	if (KdUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ks");
	if (KsUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}


	materialShineness_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_shineness");
	if (materialShineness_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	lightPositionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position");
	if (lightPositionUniform_PF == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_l_key_preesed");
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

	bLight = false;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// blue

	// set PerspectiveProjectionMatrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	// resize
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Display(void)
{

	// Light
	static GLfloat LaUniforms[] = { 0.1f, 0.1f, 0.1f };
	static GLfloat LdUniforms[] = { 1.0f, 1.0f, 1.0f };
	static GLfloat LsUniforms[] = { 1.0f, 1.0f, 1.0f };

	// Material
	static GLfloat KaUniforms[] = { 0.0f, 0.0f, 0.0f };
	static GLfloat KdUniforms[] = { 0.5f, 0.2f, 0.7f };
	static GLfloat KsUniforms[] = { 0.7f, 0.7f, 0.7f };
	static GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };	//if w(4th parameter) = 0.0f then it is directional light or if w = 1.0f then it is positional light

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	mat4 ModelMatrix = mat4::identity();
	mat4 ViewMatrix = mat4::identity();
	mat4 ProjectionMatrix = mat4::identity();


	if (chooseShader == 1)
	{
		fprintf(gpFile, "%d\n", __LINE__);
		// start using OpenGL program object
		glUseProgram(gShaderProgramObject_PV);

		if (bLight == true)
		{
			fprintf(gpFile, "%d\n", __LINE__);
			glUniform1i(LKeyPressedUniform, 1);
			glUniform3fv(LaUniform_PV, 1, (GLfloat*)LaUniforms);
			glUniform3fv(LdUniform_PV, 1, (GLfloat*)LdUniforms);
			glUniform3fv(LsUniform_PV, 1, (GLfloat*)LsUniforms);

			glUniform3fv(KaUniform_PV, 1, (GLfloat*)KaUniforms);
			glUniform3fv(KdUniform_PV, 1, (GLfloat*)KdUniforms);
			glUniform3fv(KsUniform_PV, 1, (GLfloat*)KsUniforms);

			glUniform1f(materialShineness_PV, 128.0f);
		}
		else
		{
			glUniform1i(LKeyPressedUniform, 0);
		}
		glUniform4fv(lightPositionUniform_PV, 1, (GLfloat*)lightPosition);		// It sends vector of 4 floats - sending light in an array - to send in array format use this glUniform4fv() function.

		ModelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

		glUniformMatrix4fv(gModelMatrix_PV, 1, GL_FALSE, ModelMatrix);
		glUniformMatrix4fv(gViewMatrix_PV, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(gProjectionMatrix_PV, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	}

	else if (chooseShader == 2)
	{
		glUseProgram(gShaderProgramObject_PF);

		if (bLight == true)
		{
			glUniform1i(LKeyPressedUniform, 1);
			glUniform3fv(LaUniform_PF, 1, (GLfloat*)LaUniforms);
			glUniform3fv(LdUniform_PF, 1, (GLfloat*)LdUniforms);
			glUniform3fv(LsUniform_PF, 1, (GLfloat*)LsUniforms);

			glUniform3fv(KaUniform_PF, 1, (GLfloat*)KaUniforms);
			glUniform3fv(KdUniform_PF, 1, (GLfloat*)KdUniforms);
			glUniform3fv(KsUniform_PF, 1, (GLfloat*)KsUniforms);

			glUniform1f(materialShineness_PF, 128.0f);
		}
		else
		{
			glUniform1i(LKeyPressedUniform, 0);
		}
		glUniform4fv(lightPositionUniform_PF, 1, (GLfloat*)lightPosition);		// It sends vector of 4 floats - sending light in an array - to send in array format use this glUniform4fv() function.

		
		ModelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

		glUniformMatrix4fv(gModelMatrix_PF, 1, GL_FALSE, ModelMatrix);
		glUniformMatrix4fv(gViewMatrix_PF, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(gProjectionMatrix_PF, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	}


	// *** bind vao ***
	// all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao_sphere);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	// OpenGL Drawing
	SwapBuffers(ghdc);

}

void Resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//glOrtho(left, right, bottom, top, near, far);
	gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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
	glDetachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
	// detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);

	// delete vertex shader object
	glDeleteShader(gVertexShaderObject_PV);
	gVertexShaderObject_PV = 0;

	// delete fragment shader object
	glDeleteShader(gFragmentShaderObject_PV);
	gFragmentShaderObject_PV = 0;

	// delete shader program object
	glDeleteShader(gShaderProgramObject_PV);
	gShaderProgramObject_PV = 0;

	// delete shader program object
	glDeleteProgram(gShaderProgramObject_PV);
	gShaderProgramObject_PV = 0;


	// detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);
	// detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);

	// delete vertex shader object
	glDeleteShader(gVertexShaderObject_PF);
	gVertexShaderObject_PF = 0;

	// delete fragment shader object
	glDeleteShader(gFragmentShaderObject_PF);
	gFragmentShaderObject_PF = 0;

	// delete shader program object
	glDeleteShader(gShaderProgramObject_PF);
	gShaderProgramObject_PF = 0;

	// delete shader program object
	glDeleteProgram(gShaderProgramObject_PF);
	gShaderProgramObject_PF = 0;




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
