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
FILE* gpVendorInfoFile	= 	NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev	= { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow			= false;
bool gbEscapeKeyIsPressed	= false;
bool gbFullScreen			= false;

GLuint gVertexShaderObject_PV;
GLuint gFragmentShaderObject_PV;
GLuint gShaderProgramObject_PV;

GLuint gVertexShaderObject_PF;
GLuint gFragmentShaderObject_PF;
GLuint gShaderProgramObject_PF;

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

// PER VERTEX
GLfloat lightPosition_Red_PV[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Red_PV[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Red_PV[] 		= {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightSpecular_Red_PV[] 		= {1.0f, 0.0f, 0.0f, 1.0f};

GLfloat lightPosition_Blue_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Blue_PV[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Blue_PV[] 		= {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lightSpecular_Blue_PV[] 	= {0.0f, 0.0f, 1.0f, 1.0f};

GLfloat lightPosition_Green_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Green_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Green_PV[] 	= {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lightSpecular_Green_PV[] 	= {0.0f, 1.0f, 0.0f, 1.0f};

GLfloat materialAmbient_PV[] 		= {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat materialDiffuse_PV[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular_PV[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess_PV 		= 128.0f;

// PER VERTEX
GLuint modelMatrixUniform_PV;
GLuint viewMatrixUniform_PV;
GLuint perspectiveProjectionUniform_PV;

GLuint LaUniform_Red_PV;
GLuint LdUniform_Red_PV;
GLuint LsUniform_Red_PV;
GLuint lightPositionUniform_Red_PV;

GLuint LaUniform_Blue_PV;
GLuint LdUniform_Blue_PV;
GLuint LsUniform_Blue_PV;
GLuint lightPositionUniform_Blue_PV;

GLuint LaUniform_Green_PV;
GLuint LdUniform_Green_PV;
GLuint LsUniform_Green_PV;
GLuint lightPositionUniform_Green_PV;

GLuint KaUniform_PV;
GLuint KdUniform_PV;
GLuint KsUniform_PV;
GLuint KshineUniform_PV;
GLuint LKeyPressedUniform_PV;



// PER FRAGMENT
GLfloat lightPosition_Red_PF[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Red_PF[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Red_PF[] 		= {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightSpecular_Red_PF[] 		= {1.0f, 0.0f, 0.0f, 1.0f};

GLfloat lightPosition_Blue_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Blue_PF[] 		= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Blue_PF[] 		= {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lightSpecular_Blue_PF[] 	= {0.0f, 0.0f, 1.0f, 1.0f};

GLfloat lightPosition_Green_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightAmbient_Green_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse_Green_PF[] 	= {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lightSpecular_Green_PF[] 	= {0.0f, 1.0f, 0.0f, 1.0f};

GLfloat materialAmbient_PF[] 		= {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat materialDiffuse_PF[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular_PF[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess_PF 		= 128.0f;

// PER FRAGMENT
GLuint modelMatrixUniform_PF;

GLuint viewMatrixUniform_PF;
GLuint perspectiveProjectionUniform_PF;

GLuint LaUniform_Red_PF;
GLuint LdUniform_Red_PF;
GLuint LsUniform_Red_PF;
GLuint lightPositionUniform_Red_PF;

GLuint LaUniform_Blue_PF;
GLuint LdUniform_Blue_PF;
GLuint LsUniform_Blue_PF;
GLuint lightPositionUniform_Blue_PF;

GLuint LaUniform_Green_PF;
GLuint LdUniform_Green_PF;
GLuint LsUniform_Green_PF;
GLuint lightPositionUniform_Green_PF;

GLuint KaUniform_PF;
GLuint KdUniform_PF;
GLuint KsUniform_PF;
GLuint KshineUniform_PF;
GLuint LKeyPressedUniform_PF;

bool bLight;
bool bFragmentShader = false;

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

	
	case WM_CHAR:
        switch(wParam)
        {
        	case 'Q':
        	case 'q':
        		DestroyWindow(hwnd);
        		break;

        	case VK_ESCAPE:
        		ToggleFullScreen();
        		break;

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

	        case 'V':
        	case 'v':
	            bFragmentShader = false;
	            break;

	           case 'F':
	           case 'f':
	           	bFragmentShader = true;
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
	

	//------------------------------------------------------------------------------------------
	// PER VERTEX SHADER
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//*** VERTEX SHADER ***
	//------------------------------------------------------------------------------------------

	// Create Shader
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	// Provide source code to shader
	const GLchar *vertexShaderSourceCode_PV =
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
		            "phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + diffuse_green + specular_red + specular_blue + specular_green;" \
	            "}" \
	            "else" \
	            "{" \
	            	"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
	            "}" \
            	"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
            "}";

	//gl_Position is inbuilt variable in shader
	// feed the source code to your shaderobject
	glShaderSource(gVertexShaderObject_PV, 1, (const GLchar **)&vertexShaderSourceCode_PV, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject_PV);
	
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	// shader compilation error checking

	//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	// glGetShaderiv — return a parameter from a shader object

	glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;

				// take log
				glGetShaderInfoLog(gVertexShaderObject_PV, iInfoLogLength, &written, szInfoLog);
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
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode_PV =
        "#version 440 core" \
        "\n" \
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        	"FragColor = vec4(phong_ads_light,1.0f);" \
        "}";

	glShaderSource(gFragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject_PV);

	// fragment shader compilation error checking

	
	glGetShaderiv(gFragmentShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PV, iInfoLogLength, &written, szInfoLog);
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

	GLint iShaderProgramLinkStatus = 0;

	// Reset values
	iInfoLogLength = 0;
	szInfoLog = NULL;

	// shader link error checking
	glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderProgramLinkStatus);

	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(char) * iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gShaderProgramObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}

	}

	// get MVP uniform location
	// post linking

	modelMatrixUniform_PV 				= glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
    viewMatrixUniform_PV 				= glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");
    perspectiveProjectionUniform_PV 	= glGetUniformLocation(gShaderProgramObject_PV, "u_perspective_projection_matrix");
    
    LaUniform_Red_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_la_red");
    LdUniform_Red_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_red");
    LsUniform_Red_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_ls_red");
    lightPositionUniform_Red_PV 		= glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_red");
    
    LaUniform_Blue_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_la_blue");
    LdUniform_Blue_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_blue");
    LsUniform_Blue_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_ls_blue");
    lightPositionUniform_Blue_PV 		= glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_blue");

    LaUniform_Green_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_la_green");
    LdUniform_Green_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_green");
    LsUniform_Green_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_ls_green");
    lightPositionUniform_Green_PV 		= glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_green");
    
    KaUniform_PV 						= glGetUniformLocation(gShaderProgramObject_PV, "u_ka");
    KdUniform_PV 						= glGetUniformLocation(gShaderProgramObject_PV, "u_kd");
    KsUniform_PV 						= glGetUniformLocation(gShaderProgramObject_PV, "u_ks");
    KshineUniform_PV 					= glGetUniformLocation(gShaderProgramObject_PV, "u_shininess");
    LKeyPressedUniform_PV 				= glGetUniformLocation(gShaderProgramObject_PV, "u_LKeyPressed");






	//------------------------------------------------------------------------------------------
	// PER FRAGMENT SHADER
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//*** VERTEX SHADER ***
	//------------------------------------------------------------------------------------------

	// Create Shader
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	// Provide source code to shader
	const GLchar *vertexShaderSourceCode_PF =
            "#version 440 core" \
            "\n" \
            "in vec4 vPosition;" \
            "in vec3 vNormal;" \
            "uniform mat4 u_model_matrix;" \
            "uniform mat4 u_view_matrix;" \
            "uniform mat4 u_perspective_projection_matrix;" \
            "uniform vec4 u_light_position_red;" \
            "uniform vec4 u_light_position_blue;" \
            "uniform vec4 u_light_position_green;" \
            "uniform int u_LKeyPressed;" \
            "out vec3 transformed_normal;" \
            "out vec3 view_vector;"	\
            "out vec3 light_direction_red;"	\
            "out vec3 light_direction_blue;"	\
            "out vec3 light_direction_green;"	\
            "void main(void)" \
            "{" \
	            "if(u_LKeyPressed == 1)" \
	            "{" \
		            "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		            "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		            "view_vector = -eye_coordinates.xyz;" \
		            "light_direction_red = vec3(u_light_position_red - eye_coordinates);" \
		            "light_direction_blue = vec3(u_light_position_blue - eye_coordinates);" \
 		            "light_direction_green = vec3(u_light_position_green - eye_coordinates);" \
		        "}" \
            	"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
            "}";

	//gl_Position is inbuilt variable in shader
	// feed the source code to your shaderobject
	glShaderSource(gVertexShaderObject_PF, 1, (const GLchar **)&vertexShaderSourceCode_PF, NULL);

	// compile shader
	glCompileShader(gVertexShaderObject_PF);
	
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	// shader compilation error checking

	//void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	// glGetShaderiv — return a parameter from a shader object

	glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;

				// take log
				glGetShaderInfoLog(gVertexShaderObject_PF, iInfoLogLength, &written, szInfoLog);
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
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode_PF =
        "#version 440 core" \
        "\n" \
        "in vec3 transformed_normal;" \
        "in vec3 view_vector;" \
        "in vec3 light_direction_red;" \
        "in vec3 light_direction_blue;" \
        "in vec3 light_direction_green;" \
        "uniform vec3 u_la_red;" \
        "uniform vec3 u_ld_red;" \
        "uniform vec3 u_ls_red;" \
        "uniform vec3 u_la_blue;" \
        "uniform vec3 u_ld_blue;" \
        "uniform vec3 u_ls_blue;" \
        "uniform vec3 u_la_green;" \
        "uniform vec3 u_ld_green;" \
        "uniform vec3 u_ls_green;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform int u_LKeyPressed;" \
        "uniform float u_shininess;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
	        "vec3 phong_ads_light;" \
	        "if(u_LKeyPressed == 1)" \
	        "{" \
		        "vec3 normalized_transformed_normal      = normalize(transformed_normal);" \
		        "vec3 normalized_light_direction_red     = normalize(light_direction_red);" \
		        "vec3 normalized_light_direction_blue    = normalize(light_direction_blue);" \
		        "vec3 normalized_light_direction_green   = normalize(light_direction_green);" \
		        "vec3 normalized_view_vector             = normalize(view_vector);" \
		        "vec3 reflection_vector_red = reflect(-normalized_light_direction_red,normalized_transformed_normal);" \
		        "vec3 ambient_red = u_la_red * u_ka;" \
		        "vec3 diffuse_red = u_ld_red * u_kd * max(dot(normalized_light_direction_red,normalized_transformed_normal),0.0f);" \
		        "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,normalized_view_vector ),0.0f),u_shininess);" \
		        "vec3 reflection_vector_blue = reflect(-normalized_light_direction_blue,normalized_transformed_normal);" \
		        "vec3 ambient_blue = u_la_blue * u_ka;" \
		        "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(normalized_light_direction_blue,normalized_transformed_normal),0.0f);" \
		        "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,normalized_view_vector ),0.0f),u_shininess);" \
		        "vec3 reflection_vector_green = reflect(-normalized_light_direction_green,normalized_transformed_normal);" \
		        "vec3 ambient_green = u_la_green * u_ka;" \
		        "vec3 diffuse_green = u_ld_green * u_kd * max(dot(normalized_light_direction_green,normalized_transformed_normal),0.0f);" \
		        "vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green,normalized_view_vector ),0.0f),u_shininess);" \
		        "phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + diffuse_green + specular_red + specular_blue + specular_green;" \
	        "}" \
	        "else" \
	        "{" \
	        	"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
	        "}" \
	        "FragColor = vec4(phong_ads_light,1.0f);" \
        "}";

	glShaderSource(gFragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject_PF);

	// fragment shader compilation error checking

	
	glGetShaderiv(gFragmentShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PF, iInfoLogLength, &written, szInfoLog);
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

	iShaderProgramLinkStatus = 0;

	// Reset values
	iInfoLogLength = 0;
	szInfoLog = NULL;

	// shader link error checking
	glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderProgramLinkStatus);

	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(char) * iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}

	}


	modelMatrixUniform_PF 				= glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
    viewMatrixUniform_PF 				= glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
    perspectiveProjectionUniform_PF 	= glGetUniformLocation(gShaderProgramObject_PF, "u_perspective_projection_matrix");
    
    LaUniform_Red_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_la_red");
    LdUniform_Red_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_ld_red");
    LsUniform_Red_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_ls_red");
    lightPositionUniform_Red_PF 		= glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_red");
    
    LaUniform_Blue_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_la_blue");
    LdUniform_Blue_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_ld_blue");
    LsUniform_Blue_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_ls_blue");
    lightPositionUniform_Blue_PF 		= glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_blue");

    LaUniform_Green_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_la_green");
    LdUniform_Green_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_ld_green");
    LsUniform_Green_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_ls_green");
    lightPositionUniform_Green_PF 		= glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_green");
    
    KaUniform_PF 						= glGetUniformLocation(gShaderProgramObject_PF, "u_ka");
    KdUniform_PF 						= glGetUniformLocation(gShaderProgramObject_PF, "u_kd");
    KsUniform_PF 						= glGetUniformLocation(gShaderProgramObject_PF, "u_ks");
    KshineUniform_PF 					= glGetUniformLocation(gShaderProgramObject_PF, "u_shininess");
    LKeyPressedUniform_PF 				= glGetUniformLocation(gShaderProgramObject_PF,"u_LKeyPressed");


	//------------------------------------------------------------------------------------------
	//*** SHADER PROGRAM ***
	//------------------------------------------------------------------------------------------

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

	if(bFragmentShader == true)
	{
		// start using OpenGL program object
		glUseProgram(gShaderProgramObject_PV);
		if(bLight == true)
    	{
	    	lightPosition_Red_PV[0]	= 0.0f;
	    	lightPosition_Red_PV[1]	= factor * sin(angleX);
	    	lightPosition_Red_PV[2]	= factor * cos(angleX);
	    	lightPosition_Red_PV[3]	= 1.0f;

	    	lightPosition_Green_PV[0]	= factor * sin(angleY);
	    	lightPosition_Green_PV[1]	= 0.0f;
	    	lightPosition_Green_PV[2]	= factor * cos(angleY);
	    	lightPosition_Green_PV[3]	= 1.0f;

	    	lightPosition_Blue_PV[0]	= factor * sin(angleZ);
	    	lightPosition_Blue_PV[1]	= factor * cos(angleZ);
	    	lightPosition_Blue_PV[2]	= 0.0f;
	    	lightPosition_Blue_PV[3]	= 1.0f;

	        glUniform1i(LKeyPressedUniform_PV, 1);
	        glUniform1f(KshineUniform_PV, materialShininess_PV);

	        glUniform4fv(lightPositionUniform_Red_PV, 1, lightPosition_Red_PV);
	        glUniform3fv(LaUniform_Red_PV, 1, lightAmbient_Red_PV);
	        glUniform3fv(LdUniform_Red_PV, 1, lightDiffuse_Red_PV);
	        glUniform3fv(LsUniform_Red_PV, 1, lightSpecular_Red_PV);

	        glUniform4fv(lightPositionUniform_Blue_PV, 1, lightPosition_Blue_PV);
	        glUniform3fv(LaUniform_Blue_PV, 1, lightAmbient_Blue_PV);
	        glUniform3fv(LdUniform_Blue_PV, 1, lightDiffuse_Blue_PV);
	        glUniform3fv(LsUniform_Blue_PV, 1, lightSpecular_Blue_PV);

	        glUniform4fv(lightPositionUniform_Green_PV, 1, lightPosition_Green_PV);
	        glUniform3fv(LaUniform_Green_PV, 1, lightAmbient_Green_PV);
	        glUniform3fv(LdUniform_Green_PV, 1, lightDiffuse_Green_PV);
	        glUniform3fv(LsUniform_Green_PV, 1, lightSpecular_Green_PV);

	        glUniform3fv(KaUniform_PV, 1, materialAmbient_PV);
	        glUniform3fv(KdUniform_PV, 1, materialDiffuse_PV);
	        glUniform3fv(KsUniform_PV, 1, materialSpecular_PV);
	    }
	    else
	    {
	        glUniform1i(LKeyPressedUniform_PV, 0);
	    }
	    mat4 viewMatrix = mat4::identity();
	    mat4 projectionMatrix = mat4::identity();
	    mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	    mat4 modelMatrix = mat4::identity();
	    
	    modelMatrix = TranslateMatrix;
	    projectionMatrix = gPerspectiveProjectionMatrix;

	    glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, modelMatrix);
	    glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
	    glUniformMatrix4fv(perspectiveProjectionUniform_PV, 1, GL_FALSE, projectionMatrix);
	}
	else
	{
		// start using OpenGL program object
		glUseProgram(gShaderProgramObject_PF);
		if(bLight == true)
    	{
	    	lightPosition_Red_PF[0]	= 0.0f;
	    	lightPosition_Red_PF[1]	= factor * sin(angleX);
	    	lightPosition_Red_PF[2]	= factor * cos(angleX);
	    	lightPosition_Red_PF[3]	= 1.0f;

	    	lightPosition_Green_PF[0]	= factor * sin(angleY);
	    	lightPosition_Green_PF[1]	= 0.0f;
	    	lightPosition_Green_PF[2]	= factor * cos(angleY);
	    	lightPosition_Green_PF[3]	= 1.0f;

	    	lightPosition_Blue_PF[0]	= factor * sin(angleZ);
	    	lightPosition_Blue_PF[1]	= factor * cos(angleZ);
	    	lightPosition_Blue_PF[2]	= 0.0f;
	    	lightPosition_Blue_PF[3]	= 1.0f;

	        glUniform1i(LKeyPressedUniform_PF, 1);
	        glUniform1f(KshineUniform_PF, materialShininess_PF);

	        glUniform4fv(lightPositionUniform_Red_PF, 1, lightPosition_Red_PF);
	        glUniform3fv(LaUniform_Red_PF, 1, lightAmbient_Red_PF);
	        glUniform3fv(LdUniform_Red_PF, 1, lightDiffuse_Red_PF);
	        glUniform3fv(LsUniform_Red_PF, 1, lightSpecular_Red_PF);

	        glUniform4fv(lightPositionUniform_Blue_PF, 1, lightPosition_Blue_PF);
	        glUniform3fv(LaUniform_Blue_PF, 1, lightAmbient_Blue_PF);
	        glUniform3fv(LdUniform_Blue_PF, 1, lightDiffuse_Blue_PF);
	        glUniform3fv(LsUniform_Blue_PF, 1, lightSpecular_Blue_PF);

	        glUniform4fv(lightPositionUniform_Green_PF, 1, lightPosition_Green_PF);
	        glUniform3fv(LaUniform_Green_PF, 1, lightAmbient_Green_PF);
	        glUniform3fv(LdUniform_Green_PF, 1, lightDiffuse_Green_PF);
	        glUniform3fv(LsUniform_Green_PF, 1, lightSpecular_Green_PF);

	        glUniform3fv(KaUniform_PF, 1, materialAmbient_PF);
	        glUniform3fv(KdUniform_PF, 1, materialDiffuse_PF);
	        glUniform3fv(KsUniform_PF, 1, materialSpecular_PF);
	    }
	    else
	    {
	        glUniform1i(LKeyPressedUniform_PF, 0);
	    }
	    mat4 viewMatrix = mat4::identity();
	    mat4 projectionMatrix = mat4::identity();
	    mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	    mat4 modelMatrix = mat4::identity();
	    
	    modelMatrix = TranslateMatrix;
	    projectionMatrix = gPerspectiveProjectionMatrix;

	    glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, modelMatrix);
	    glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
	    glUniformMatrix4fv(perspectiveProjectionUniform_PF, 1, GL_FALSE, projectionMatrix);
	}
	
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

	if(gShaderProgramObject_PV)
	{
		glUseProgram(gShaderProgramObject_PV);
		GLsizei shaderCount;

		glGetProgramiv(gShaderProgramObject_PV, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders = NULL;

		pShaders = (GLuint*)malloc(shaderCount * sizeof(GLuint));
		if(pShaders == NULL)
		{
			printf("Malloc Failed. Exitting now...\n\n");
			exit(0);
		}

		glGetAttachedShaders(gShaderProgramObject_PV, shaderCount, &shaderCount, pShaders);

		for(GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_PV, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
			free(pShaders);

			glDeleteProgram(gShaderProgramObject_PV);
			gShaderProgramObject_PV = 0;
			glUseProgram(0);
		}
	}

	if(gShaderProgramObject_PF)
	{
		glUseProgram(gShaderProgramObject_PF);
		GLsizei shaderCount;

		glGetProgramiv(gShaderProgramObject_PF, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders = NULL;

		pShaders = (GLuint*)malloc(shaderCount * sizeof(GLuint));
		if(pShaders == NULL)
		{
			printf("Malloc Failed. Exitting now...\n\n");
			exit(0);
		}

		glGetAttachedShaders(gShaderProgramObject_PF, shaderCount, &shaderCount, pShaders);

		for(GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_PF, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
			free(pShaders);

			glDeleteProgram(gShaderProgramObject_PF);
			gShaderProgramObject_PF = 0;
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
