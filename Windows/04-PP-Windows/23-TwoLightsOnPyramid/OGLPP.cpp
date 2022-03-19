#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>

#include<gl/glew.h>
#include<gl/gl.h>

#include "vmath.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
vmath;

enum
{
    SSK_ATTRIBUT
using namespace E_POSITION = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXCORD,
};

// Global variable declarations
FILE* gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

bool bLight;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLfloat lightPosition_Red[] = {-2.0f,0.0,0.0f,1.0f};
GLfloat lightAmbient_Red[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_Red[] = {1.0f,0.0f,0.0f,1.0f};
GLfloat lightSpecular_Red[] = {1.0f,0.0f,0.0f,1.0f};

GLfloat lightPosition_Blue[] = {2.0f,0.0,0.0f,1.0f};
GLfloat lightAmbient_Blue[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_Blue[] = {0.0f,0.0f,1.0f,1.0f};
GLfloat lightSpecular_Blue[] = {0.0f,0.0f,1.0f,1.0f};

GLfloat materialAmbient[] = {0.0f,0.0f,0.0f,0.0f};
GLfloat materialDiffuse[] = {1.0f,1.0f,1.0f,1.0f};
GLfloat materialSpecular[] = {1.0f,1.0f,1.0f,1.0f};
GLfloat materialShininess = 128.0f;

// pyramid
GLuint gVao_Pyramid;
GLuint gVbo_Position_Pyramid;
GLuint gVbo_Normal_Pyramid;

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

GLuint KaUniform;
GLuint KdUniform;
GLuint KsUniform;
GLuint KshineUniform;
GLuint LKeyPressedUniform;

mat4 gPerspectiveProjectionMatrix;

GLfloat pyramidAngle = 0.0f;

// Prototype of WndProc() delclared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
            if(bLight == true)
            {
                fprintf(gpFile, "Pressed blight == true.\n");
                bLight = false;
            }
            else
            {
                fprintf(gpFile, "Pressed blight == false.\n");
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

    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if (iPixelFormatIndex == 0)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    ghrc = wglCreateContext(ghdc);
    if (ghrc == NULL)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    if (wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        wglDeleteContext(ghrc);
        ghrc = NULL;
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    GLenum glew_error = glewInit();
    if (glew_error != GLEW_OK)
    {
        wglDeleteContext(ghrc);
        ghrc = NULL;
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    //vertex shader
    // create shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //Feed Shader
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
            "vec3 reflection_vector_red = reflect(-light_direction_red,transformed_normal);" \
            "vec3 ambient_red = u_la_red * u_ka;" \
            "vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red,transformed_normal),0.0f);" \
            "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,view_vector),0.0f),u_shininess);" \
            "vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" \
            "vec3 reflection_vector_blue = reflect(-light_direction_blue,transformed_normal);" \
            "vec3 ambient_blue = u_la_blue * u_ka;" \
            "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue,transformed_normal),0.0f);" \
            "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,view_vector),0.0f),u_shininess);" \
            "phong_ads_light = ambient_red + ambient_blue + diffuse_red + diffuse_blue + specular_red + specular_blue;" \
            "}" \
            "else" \
            "{" \
            "phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
            "}" \
            "gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
            "}";
    glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gVertexShaderObject);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;

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
                glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                UnInitialize();
                exit(0);
            }
        }
    }

    // Fragment shader
    // crete shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
     //Feed Shader
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

    // Error checking
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
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                UnInitialize();
                exit(0);
            }
        }
    }

    // shader program
    // create program
    gShaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    //glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");

    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

    // link shader
    glLinkProgram(gShaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
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

    // get MVP Uniform location
    modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
    viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
    perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
    LaUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_la_red");
    LdUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_ld_red");
    LsUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_ls_red");
    lightPositionUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_light_position_red");
    LaUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_la_blue");
    LdUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_ld_blue");
    LsUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_ls_blue");
    lightPositionUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_light_position_blue");
    KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    KshineUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");
    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject,"u_LKeyPressed");

    const GLfloat pyramidVertices[] = 
    {
        +0.0f,  +0.5f,  +0.0f,
        -0.5f,  -0.5f,  +0.5f,
        +0.5f,  -0.5f,  +0.5f,

        +0.0f,  +0.5f,  +0.0f,
        +0.5f,  -0.5f,  +0.5f,
        +0.5f,  -0.5f,  -0.5f,

        +0.0f,  +0.5f,  +0.0f,
        +0.5f,  -0.5f,  -0.5f,
        -0.5f,  -0.5f,  -0.5f,

        +0.0f,  +0.5f,  +0.0f,
        -0.5f,  -0.5f,  -0.5f,
        -0.5f,  -0.5f,  +0.5f
    };

    const GLfloat pyramidNormals[]  =
    {
        //FRONT FACE
        0.0f,   0.447214f,  0.894427f,
        0.0f,   0.447214f,  0.894427f,
        0.0f,   0.447214f,  0.894427f,
        0.0f,   0.447214f,  0.894427f,
        
        //RIGHT FACE
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        
        //BACK FACE
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        
        //LEFT FACE
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f

    };

    glGenVertexArrays(1, &gVao_Pyramid);
    glBindVertexArray(gVao_Pyramid);

        glGenBuffers(1, &gVbo_Position_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // position
        glGenBuffers(1, &gVbo_Normal_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glShadeModel(GL_SMOOTH);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    bLight = false;

    gPerspectiveProjectionMatrix = mat4::identity();

    Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Display(void)
{
    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // start using opengl program object
    glUseProgram(gShaderProgramObject);

    if(bLight == true)
    {
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
    mat4 RotationMatrix = vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);
    
    modelMatrix = TranslateMatrix * RotationMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, projectionMatrix);

    // bind vao
    glBindVertexArray(gVao_Pyramid);
        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        glDrawArrays(GL_TRIANGLES, 0, 12); // 12 total vertex
    // unbind vao
    glBindVertexArray(0);

    // stop using opengl program object
    glUseProgram(0);

    pyramidAngle += 0.1f;
    if (pyramidAngle >= 360.0f)
        pyramidAngle = 0.0f;

    // OpenGL Drawing
    SwapBuffers(ghdc);
}

void Resize(int width, int height)
{
    if (height == 0)
        height = 1;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
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
    if (gVao_Pyramid)
    {
        glDeleteVertexArrays(1, &gVao_Pyramid);
        gVao_Pyramid = 0;
    }

    // destroy vbo
    if (gVbo_Position_Pyramid)
    {
        glDeleteBuffers(1, &gVbo_Position_Pyramid);
        gVbo_Position_Pyramid = 0;
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
