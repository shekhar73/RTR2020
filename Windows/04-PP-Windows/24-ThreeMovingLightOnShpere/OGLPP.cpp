#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/GL.h>

#include "Sphere.h"
#include "vmath.h"	// It is a C++ file

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

GLfloat angleX = 0.0f;
GLfloat angleY = 0.0f;
GLfloat angleZ = 0.0f;

GLfloat lightPosition_Red[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Red[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Red[] 		= {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightSpecular_Red[] 	= {1.0f, 0.0f, 0.0f, 1.0f};

GLfloat lightPosition_Blue[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Blue[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Blue[] 	= {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lightSpecular_Blue[] 	= {0.0f, 0.0f, 1.0f, 1.0f};

GLfloat lightPosition_Green[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Green[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Green[] 	= {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lightSpecular_Green[] 	= {0.0f, 1.0f, 0.0f, 1.0f};

GLfloat materialAmbient[] 		= {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat materialDiffuse[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess 		= 128.0f;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint perspectiveProjectionUniform;

GLuint LaUniform_Red;
GLuint LdUniform_Red;
GLuint LsUniform_Red;
GLuint lightPositionUniform_Red;

GLuint LaUniform_Blue;
GLuint LdUniform_Blue;
GLuint LsUniform_Blue;
GLuint lightPositionUniform_Blue;

GLuint LaUniform_Green;
GLuint LdUniform_Green;
GLuint LsUniform_Green;
GLuint lightPositionUniform_Green;

GLuint KaUniform;
GLuint KdUniform;
GLuint KsUniform;
GLuint KshineUniform;
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

		case WM_CHAR:
        switch(wParam)
        {
        	case 'L':
        	case 'l':
	            if(bLight == true)
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
	MONITORINFO mi = { sizeof(MONITORINFO) };

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOZORDER);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;
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

	// Create Shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// Provide source code to shader
	const GLchar *vertexShaderSourceCode =
            "#version 440 core" \
            "\n" \
            "in vec4 vPosition;" \
            "in vec3 vNormal;" \
            "uniform mat4 u_model_matrix;" \
            "uniform mat4 u_view_matrix;" \
            "uniform mat4 u_perspective_projection_matrix;" \
            "uniform vec3 u_la_red;" \
            "uniform vec3 u_ld_red;" \
            "uniform vec3 u_ls_red;" \
            "uniform vec4 u_light_position_red;" \
            "uniform vec3 u_la_blue;" \
            "uniform vec3 u_ld_blue;" \
            "uniform vec3 u_ls_blue;" \
            "uniform vec4 u_light_position_blue;" \
            "uniform vec3 u_la_green;" \
            "uniform vec3 u_ld_green;" \
            "uniform vec3 u_ls_green;" \
            "uniform vec4 u_light_position_green;" \
            "uniform vec3 u_ka;" \
            "uniform vec3 u_kd;" \
            "uniform vec3 u_ks;" \
            "uniform int u_LKeyPressed;" \
            "uniform float u_shininess;" \
            "out vec3 phong_ads_light;" \
            "void main(void)" \
            "{" \
	            "if(u_LKeyPressed == 1)" \
	            "{" \
		            "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		            "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		            "vec3 view_vector = normalize(-eye_coordinates.xyz);" \
		            "vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));" \
		            "vec3 reflection_vector_red = reflect(-light_direction_red, transformed_normal);" \
		            "vec3 ambient_red = u_la_red * u_ka;" \
		            "vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red,transformed_normal),0.0f);" \
		            "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, view_vector),0.0f),u_shininess);" \
		            "vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" \
		            "vec3 reflection_vector_blue = reflect(-light_direction_blue, transformed_normal);" \
		            "vec3 ambient_blue = u_la_blue * u_ka;" \
		            "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue,transformed_normal),0.0f);" \
		            "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, view_vector),0.0f),u_shininess);" \
					"vec3 light_direction_green = normalize(vec3(u_light_position_green - eye_coordinates));" \
		            "vec3 reflection_vector_green = reflect(-light_direction_green, transformed_normal);" \
		            "vec3 ambient_green = u_la_green * u_ka;" \
		            "vec3 diffuse_green = u_ld_green * u_kd * max(dot(light_direction_green, transformed_normal),0.0f);" \
		            "vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green, view_vector),0.0f),u_shininess);" \
		            "phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + +diffuse_green + specular_red + specular_blue + specular_green;" \
	            "}" \
	            "else" \
	            "{" \
	            	"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
	            "}" \
            	"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
	const GLchar *fragmentShaderSourceCode =
        "#version 440 core" \
        "\n" \
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        	"FragColor = vec4(phong_ads_light,1.0f);" \
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
	modelMatrixUniform 				= glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
    viewMatrixUniform 				= glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
    perspectiveProjectionUniform 	= glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
    
    LaUniform_Red 					= glGetUniformLocation(gShaderProgramObject, "u_la_red");
    LdUniform_Red 					= glGetUniformLocation(gShaderProgramObject, "u_ld_red");
    LsUniform_Red 					= glGetUniformLocation(gShaderProgramObject, "u_ls_red");
    lightPositionUniform_Red 		= glGetUniformLocation(gShaderProgramObject, "u_light_position_red");
    
    LaUniform_Blue 					= glGetUniformLocation(gShaderProgramObject, "u_la_blue");
    LdUniform_Blue 					= glGetUniformLocation(gShaderProgramObject, "u_ld_blue");
    LsUniform_Blue 					= glGetUniformLocation(gShaderProgramObject, "u_ls_blue");
    lightPositionUniform_Blue 		= glGetUniformLocation(gShaderProgramObject, "u_light_position_blue");

    LaUniform_Green 				= glGetUniformLocation(gShaderProgramObject, "u_la_green");
    LdUniform_Green 				= glGetUniformLocation(gShaderProgramObject, "u_ld_green");
    LsUniform_Green 				= glGetUniformLocation(gShaderProgramObject, "u_ls_green");
    lightPositionUniform_Green 		= glGetUniformLocation(gShaderProgramObject, "u_light_position_green");
    
    KaUniform 						= glGetUniformLocation(gShaderProgramObject, "u_ka");
    KdUniform 						= glGetUniformLocation(gShaderProgramObject, "u_kd");
    KsUniform 						= glGetUniformLocation(gShaderProgramObject, "u_ks");
    KshineUniform 					= glGetUniformLocation(gShaderProgramObject, "u_shininess");
    LKeyPressedUniform 				= glGetUniformLocation(gShaderProgramObject,"u_LKeyPressed");

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
	
	// It will store all the below 6 steps so that we can reuse it in draw function
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);
	
	glGenBuffers(1, &gVbo_sphere_position);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 	// Unbind the BUFFER


	// Normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);	//like glGenTextures() in FFP
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);	// like glBindTexture() in FFP
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 	// Unbind the BUFFER


	// Element vbo
	glGenBuffers(1, &gVbo_sphere_element);	//like glGenTextures() in FFP
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);	// like glBindTexture() in FFP
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 	// Unbind the BUFFER

	// Unbind Vao
	glBindVertexArray(0);

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
	glUseProgram(gShaderProgramObject);

	if(bLight == true)
    {
    	lightPosition_Red[0]	= 0.0f;
    	lightPosition_Red[1]	= factor * sin(angleX);
    	lightPosition_Red[2]	= factor * cos(angleX);
    	lightPosition_Red[3]	= 1.0f;

    	lightPosition_Green[0]	= factor * sin(angleY);
    	lightPosition_Green[1]	= 0.0f;
    	lightPosition_Green[2]	= factor * cos(angleY);
    	lightPosition_Green[3]	= 1.0f;

    	lightPosition_Blue[0]	= factor * sin(angleZ);
    	lightPosition_Blue[1]	= factor * cos(angleZ);
    	lightPosition_Blue[2]	= 0.0f;
    	lightPosition_Blue[3]	= 1.0f;

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);

        glUniform4fv(lightPositionUniform_Red, 1, lightPosition_Red);
        glUniform3fv(LaUniform_Red, 1, lightAmbient_Red);
        glUniform3fv(LdUniform_Red, 1, lightDiffuse_Red);
        glUniform3fv(LsUniform_Red, 1, lightSpecular_Red);

        glUniform4fv(lightPositionUniform_Blue, 1, lightPosition_Blue);
        glUniform3fv(LaUniform_Blue, 1, lightAmbient_Blue);
        glUniform3fv(LdUniform_Blue, 1, lightDiffuse_Blue);
        glUniform3fv(LsUniform_Blue, 1, lightSpecular_Blue);

        glUniform4fv(lightPositionUniform_Green, 1, lightPosition_Green);
        glUniform3fv(LaUniform_Green, 1, lightAmbient_Green);
        glUniform3fv(LdUniform_Green, 1, lightDiffuse_Green);
        glUniform3fv(LsUniform_Green, 1, lightSpecular_Green);

        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffuse);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    // opengl drawing
    // set modelview & modelviewprojection matrix to identity
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();
    mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    
    modelMatrix = TranslateMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, projectionMatrix);

	// *** bind vao ***
	// all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao_sphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	// *** unbind vao ***
	glBindVertexArray(0);

	// Stop using OpenGL program object
	glUseProgram(0);

	angleX = angleX + 0.01f;
	if(angleX >= 360.0f)
	{
		angleX = 0.0f;
	}

	angleY = angleY + 0.01f;
	if(angleY >= 360.0f)
	{
		angleY = 0.0f;
	}

	angleZ = angleZ + 0.01f;
	if(angleZ >= 360.0f)
	{
		angleZ = 0.0f;
	}

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

	if(gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);
		GLsizei shaderCount;

		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders = NULL;

		pShaders = (GLuint*)malloc(shaderCount * sizeof(GLuint));
		if(pShaders == NULL)
		{
			printf("Malloc Failed. Exitting now...\n\n");
			exit(0);
		}

		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

		for(GLsizei i = 0; i < shaderCount; i++)
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

	
	if(wglGetCurrentContext() == ghrc)
	{
		// Deselect the rendering context
		wglMakeCurrent(NULL, NULL);
	}
	
	if(ghrc)
	{
		// delete the rendering context
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	
	if(ghdc)
	{
		// Delete the device context
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
