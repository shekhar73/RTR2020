#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/glew.h>

#include<GL/gl.h> // for OPenGL
#include<GL/glx.h>	// for glx API

#include<SOIL/SOIL.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include"vmath.h"
#include "Sphere.h"

// namespaces
using namespace std;
using namespace vmath;

enum
{
	SSK_ATTRIBUTE_POSITION = 0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXCOORD,
};

// global variable
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;
GLXContext gGLXContext;

typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);  
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;

char keys[26];

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[746];
unsigned short sphere_elements[2280];
int gNumSphereVertices;
int gNumSphereElements;

GLuint vao;  
GLuint vbo_position;
GLuint vbo_normal;
GLuint vbo_element;

// Per Vertex Light Model
GLuint gShaderProgramObjectPF;
GLuint gVertexShaderObjectPF;
GLuint gFragmentShaderObjectPF;

GLuint modelMatrixUniformPF;
GLuint perspectiveProjectionUniformPF;
GLuint viewMatrixUniformPF;

GLuint LaUniformPF;
GLuint LsUniformPF;
GLuint LdUniformPF;
GLuint LpUniformPF; 

GLuint KaUniformPF;
GLuint KdUniformPF;
GLuint KsUniformPF;
GLfloat MaterialShineUniformPF; 

GLuint lKeyPressedUniformPF;

GLuint LightaUniformPF[3];
GLuint LightsUniformPF[3];
GLuint LightdUniformPF[3];
GLuint LightpUniformPF[3]; 

GLfloat lightAngle0 = 0.0f;
GLfloat lightAngle1 = 0.0f;
GLfloat lightAngle2 = 0.0f;

bool bAnimate = false;
bool bLight = false;
bool gbLightModelToggle = false;

mat4 perspectiveProjectionMatrix;

FILE* gpFile = NULL;

// entry-point function
int main()
{
	// function declaration
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void Initialize(void);
	void Resize(int, int);
	void Draw(void);
	void UnInitialize();

	// variable declaration
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;
	bool bDone = false;

	// code
	CreateWindow();

	Initialize();

	// message loop
	XEvent event;
	KeySym keysym;
				
	while(bDone == false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);
			switch(event.type)
			{
				case MapNotify:
				break;

				case KeyPress:
				keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
		
				switch(keysym)
				{
					case XK_Escape:
					bDone = true;
					break;

					default:
					break;
				}

				
				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
							
				switch (keys[0]) 
				{
					case 'F':
					case 'f':
					if (bFullscreen == false) 
					{
						ToggleFullscreen();
						bFullscreen = true;
					}
					else 
					{
						ToggleFullscreen();
						bFullscreen = false;
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
                    
                    case 'A':
                    case 'a':
                    if (bAnimate == true) 
                    {
                       	bAnimate = false;
                    }
                    else 
                    {
                        bAnimate = true;
                    }
                    break;

					default:
						break;
				}
				break;

				case ButtonPress:
					switch (event.xbutton.button)
					{
						case 1:
							break;
				
						case 2:
							break;
						
						case 3:
							break;
						
						case 4:
							break;
						
						default:
							break;
					}
					break;
				
				case MotionNotify:
				break;

				case ConfigureNotify:
				winWidth = event.xconfigure.width;
				winHeight = event.xconfigure.height;
				Resize(winWidth, winHeight);
				break;

				case Expose:
				break;

				case DestroyNotify:
				break;

				case 33:
				bDone = true;
				break;
			}
		}

		Draw();
	}

	UnInitialize();

	return(0);
}

void CreateWindow(void)
{
   void UnInitialize(void);

    // variable declarations
    XSetWindowAttributes winAttribs;
    int defaultScreen;
    int styleMask;
    
    GLXFBConfig *pGLXFBConfig = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo *pTempXVisualInfo = NULL;
    int numFBConfigs = 0;
    
    
    
    static int frameBufferAttributes[] = 
    {
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24, 
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        GLX_X_RENDERABLE, True,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        None
        
    };
    
    // Code
    gpDisplay = XOpenDisplay(NULL);
    if(gpDisplay == NULL)
    {
        printf("Error : Unable to Open X Display.\nExitting Now..\n");
        UnInitialize();
        exit(1);
    }

    defaultScreen = XDefaultScreen(gpDisplay);
    
    
    pGLXFBConfig = glXChooseFBConfig(gpDisplay, defaultScreen, frameBufferAttributes, &numFBConfigs);
    printf("Found number of FBCOnfigs : %d\n",numFBConfigs);
    int bestFrameBufferConfig = -1;
    int worstFrameBufferConfig = -1;
    int bestNumberOfSamples = -1;
    int worstNumberOfSamples = 999;
    for(int i=0; i<numFBConfigs; i++)
    {
        pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
        if(pTempXVisualInfo != NULL)
        {
            int sampleBuffers, samples;
            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
            
            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);
            
            if(bestFrameBufferConfig < 0 || sampleBuffers && samples>bestNumberOfSamples)
            {
                bestFrameBufferConfig = i;
                bestNumberOfSamples = samples;
            }
            
            if(worstFrameBufferConfig < 0 || !sampleBuffers || samples<worstNumberOfSamples)
            {
                worstFrameBufferConfig = i;
                worstNumberOfSamples = samples;
            }
            
            
        }
        
        XFree(pTempXVisualInfo);
    }
    
    bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];
    gGLXFBConfig = bestGLXFBConfig;
    XFree(pGLXFBConfig);
    
    gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, gGLXFBConfig);
    if(gpXVisualInfo == NULL)
    {
        printf("Error : Unable to get a Visual.\nExiting Now..\n");
        UnInitialize();
        exit(1);
    }

    winAttribs.border_pixel=0;
    winAttribs.background_pixmap=0;

    winAttribs.colormap = XCreateColormap(gpDisplay,
                            RootWindow(gpDisplay, gpXVisualInfo->screen),
                            gpXVisualInfo->visual,
                            AllocNone);
    
    
    gColormap=winAttribs.colormap;
    winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
    winAttribs.event_mask= ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
    styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
    gWindow=XCreateWindow(gpDisplay,
		                    RootWindow(gpDisplay,gpXVisualInfo->screen),
		                    0,
		                    0,
		                    giWindowWidth,
		                    giWindowHeight,
		                    0,
		                    gpXVisualInfo->depth,
		                    InputOutput,
		                    gpXVisualInfo->visual,
		                    styleMask,
		                    &winAttribs);

    

    if(!gWindow)
    {
        printf("Error : Failed to create main window.\nExiting now...\n");
        UnInitialize();
        exit(1);
    }

    XStoreName(gpDisplay,gWindow,"SSK");

    Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True); // True -> XWindows type Boolean
    XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

    XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{
	// variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};

	// code
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(xev));

	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;

	XSendEvent(gpDisplay,
				RootWindow(gpDisplay, gpXVisualInfo->screen),
				False,
				StructureNotifyMask,
				&xev);
}

void Initialize(void)
{
	void Resize(int, int);
	void UnInitialize(void);
	void getSphereVertexData(float[], float[], float[], unsigned short[]);
    int getNumberOfSphereVertices(void);
    int getNumberOfSphereElements(void);

 glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
    const int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };
    
    gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
    if(!gGLXContext){
        
        const int attribsLow[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        }; 
        gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribsLow);
        printf("gGLX Context Low\n");
    }
    
    Bool isDirectContext = glXIsDirect(gpDisplay, gGLXContext);
    if(isDirectContext){
        printf("Direct Hardware Rendering Context obtained\n");
    }
    else{
        printf("Software Rendering Context obtained\n");
    }
    
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
    
    
    printf("OpenGL Vendor      :%s\n", glGetString(GL_VENDOR));
	printf("OpenGL Renderer    :%s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version     :%s\n", glGetString(GL_VERSION));
	printf("GLSL Version       :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
	GLenum err = glewInit();


	
    gVertexShaderObjectPF = glCreateShader(GL_VERTEX_SHADER);
	
	
	const GLchar* vertexShaderSourceCode =
		
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_lKeyPressed;" \
		"uniform vec4 u_lightp[3];" \
		"out vec3 transformed_Normal;" \
		"out vec3 light_direction[3];" \
		"out vec3 view_vector;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_lKeyPressed == 1){" \
		"vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
		"transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"for (int i = 0; i < 3; i++) { "\
		"light_direction[i] = vec3(u_lightp[i] - eyeCoordinate);" \
		"}" \
		"view_vector = -eyeCoordinate.xyz;" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
		"}";
	
	glShaderSource(gVertexShaderObjectPF, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
        
	
	glCompileShader(gVertexShaderObjectPF);
	
	GLint infoLogLength = 0;
	GLint shaderCompileStatus = 0;
	char* szBuffer = NULL;

	
	glGetShaderiv(gVertexShaderObjectPF, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
			if (szBuffer != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectPF, infoLogLength, &written, szBuffer);

				printf("Vertex Shader Compilation Log : %s\n", szBuffer);
				XFree(szBuffer);
				UnInitialize();
                exit(1);
			}
		}
	}

	
	gFragmentShaderObjectPF = glCreateShader(GL_FRAGMENT_SHADER);
	
	
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"vec3 phong_ads_light;" \
		"in vec3 transformed_Normal;" \
		"in vec3 light_direction[3];" \
		"in vec3 view_vector;" \
		"out vec4 FragColor;" \
		"uniform vec3 u_lightd[3];" \
		"uniform vec3 u_lighta[3];" \
		"uniform vec3 u_lights[3];" \
		"uniform vec4 u_lightp[3];" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ks;" \
		"uniform float materialShineUniform;" \
		"uniform int u_lKeyPressed;" \
		"void main(void)" \
		"{" \
		"if(u_lKeyPressed == 1){" \
		"vec3 normalizedTransformNormal = normalize(transformed_Normal);" \
		"vec3 normalizedViewVector = normalize(view_vector);" \
		"for (int i = 0; i < 3; i++) { " \
		"vec3 normalizedLightDirection = normalize(light_direction[i]);" \
		"vec3 ambient = u_lighta[i] * u_ka;" \
		"vec3 diffuse = u_lightd[i] * u_kd * max(dot(normalizedLightDirection, normalizedTransformNormal) , 0.0f);" \
		"vec3 reflection_vector = reflect(-normalizedLightDirection, normalizedTransformNormal);" \
		"vec3 specular = u_lights[i] * u_ks * pow(max(dot(reflection_vector, normalizedViewVector), 0.0f) ,  materialShineUniform);" \
		"phong_ads_light = phong_ads_light + ambient + diffuse + specular;" \
		"}" \
		"}" \
		"else{" \
		"phong_ads_light = vec3(1.0,1.0,1.0f);" \
		"}" \
		"FragColor = vec4(phong_ads_light, 1.0);" \
		"}";
	
	glShaderSource(gFragmentShaderObjectPF, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	
	glCompileShader(gFragmentShaderObjectPF);
	
	
	infoLogLength = 0;
	shaderCompileStatus = 0;
	szBuffer = NULL;
	
	glGetShaderiv(gFragmentShaderObjectPF, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragmentShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
			if (szBuffer != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectPF, infoLogLength, &written, szBuffer);

				printf("Fragment Shader Compilation Log : %s\n", szBuffer);
				
				UnInitialize();
                exit(1);
			}
		}
	}

	// Shader Linking Code
	// *** Shader Program ***
	
	gShaderProgramObjectPF = glCreateProgram();

	 
					
	glAttachShader(gShaderProgramObjectPF, gVertexShaderObjectPF);
	
	glAttachShader(gShaderProgramObjectPF, gFragmentShaderObjectPF);
    
	glBindAttribLocation(gShaderProgramObjectPF, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObjectPF, SSK_ATTRIBUTE_NORMAL, "vNormal");
    
	
	glLinkProgram(gShaderProgramObjectPF);
	
	infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;
	glGetProgramiv(gShaderProgramObjectPF, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
			if (szBuffer != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPF, infoLogLength, &written, szBuffer);

				printf("Shader Program Log : %s\n", szBuffer);
				
				UnInitialize();
                exit(1);
			}
		}
	}

	modelMatrixUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_modelMatrix");
	viewMatrixUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_viewMatrix");
	perspectiveProjectionUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_projectionMatrix");
	lKeyPressedUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_lKeyPressed");

	
	LightdUniformPF[0] = glGetUniformLocation(gShaderProgramObjectPF, "u_lightd[0]");
	LightaUniformPF[0] = glGetUniformLocation(gShaderProgramObjectPF, "u_lighta[0]");
	LightsUniformPF[0] = glGetUniformLocation(gShaderProgramObjectPF, "u_lights[0]");
	LightpUniformPF[0] = glGetUniformLocation(gShaderProgramObjectPF, "u_lightp[0]");
	
	LightdUniformPF[1] = glGetUniformLocation(gShaderProgramObjectPF, "u_lightd[1]");
	LightaUniformPF[1] = glGetUniformLocation(gShaderProgramObjectPF, "u_lighta[1]");
	LightsUniformPF[1] = glGetUniformLocation(gShaderProgramObjectPF, "u_lights[1]");
	LightpUniformPF[1] = glGetUniformLocation(gShaderProgramObjectPF, "u_lightp[1]");
	
	LightdUniformPF[2] = glGetUniformLocation(gShaderProgramObjectPF, "u_lightd[2]");
	LightaUniformPF[2] = glGetUniformLocation(gShaderProgramObjectPF, "u_lighta[2]");
	LightsUniformPF[2] = glGetUniformLocation(gShaderProgramObjectPF, "u_lights[2]");
	LightpUniformPF[2] = glGetUniformLocation(gShaderProgramObjectPF, "u_lightp[2]");

	KdUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_kd");
	KaUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ka");
	KsUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ks");
	MaterialShineUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "materialShineUniform");
	 
    // Sphere Data
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumSphereVertices = getNumberOfSphereVertices();
	gNumSphereElements = getNumberOfSphereElements();
    
    
    
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Position | Vertex
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Normal
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &vbo_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    perspectiveProjectionMatrix = mat4::identity();
    
    Resize(giWindowWidth, giWindowHeight);
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Draw(void)
{
 	GLfloat lightAmbient0[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat lightDiffuse0[] = { 1.0f,0.0f,0.0f,1.0f };
	GLfloat lightSpecular0[] = { 1.0f,0.0f,0.0f,1.0f };
	GLfloat lightPosition0[4];

	
	GLfloat lightAmbient1[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat lightDiffuse1[] = { 0.0f,0.0f,1.0f,1.0f };
	GLfloat lightSpecular1[] = { 0.0f,0.0f,1.0f,1.0f };
	GLfloat lightPosition1[4];

	
	GLfloat lightAmbient2[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat lightDiffuse2[] = { 0.0f,1.0f,0.0f,1.0f };
	GLfloat lightSpecular2[] = { 0.0f,1.0f,0.0f,1.0f };
	GLfloat lightPosition2[4];

	
	GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat materialShine = 128.0f;
    
    
    mat4 translateMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationXMatrix = mat4::identity();
    mat4 rotationYMatrix = mat4::identity();
    mat4 rotationZMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

    // code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (bAnimate == true) 
    {
		lightPosition0[0] = 0.0f;
		lightPosition0[1] = 8.0f * sin(lightAngle0);
		lightPosition0[2] = 8.0f * cos(lightAngle0);
		lightPosition0[3] = 1.0f;

		
		lightPosition1[0] = 8.0f * sin(lightAngle1);
		lightPosition1[1] = 0.0f;
		lightPosition1[2] = 8.0f * cos(lightAngle1);
		lightPosition1[3] = 1.0f;

		lightPosition2[0] = 8.0f * cos(lightAngle2);
		lightPosition2[1] = 8.0f * sin(lightAngle2);
		lightPosition2[2] = 0.0f;
		lightPosition2[3] = 1.0f;

		lightAngle0 = lightAngle0 + 0.01f;
		if (lightAngle0 >= 360.0f) 
		{
			lightAngle0 = 0.0f;
		}
		lightAngle1 = lightAngle1 + 0.01f;
		if (lightAngle1 >= 360.0f) 
		{
			lightAngle1 = 0.0f;
		}
		lightAngle2 = lightAngle2 + 0.01f;
		if (lightAngle2 >= 360.0f) 
		{
			lightAngle2 = 0.0f;
		}
	}
	
	
    
	glUseProgram(gShaderProgramObjectPF);
    

    translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translateMatrix;
	
    glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniformPF, 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(viewMatrixUniformPF, 1, GL_FALSE, viewMatrix);
    
    if(bLight == true)
    {
        glUniform1i(lKeyPressedUniformPF, 1);

		glUniform3fv(LightdUniformPF[0], 1, lightDiffuse0);
		glUniform3fv(LightaUniformPF[0], 1, lightAmbient0);
		glUniform3fv(LightsUniformPF[0], 1, lightSpecular0);
		glUniform4fv(LightpUniformPF[0], 1, lightPosition0);

		glUniform3fv(LightdUniformPF[1], 1, lightDiffuse1);
		glUniform3fv(LightaUniformPF[1], 1, lightAmbient1);
		glUniform3fv(LightsUniformPF[1], 1, lightSpecular1);
		glUniform4fv(LightpUniformPF[1], 1, lightPosition1);

		glUniform3fv(LightdUniformPF[2], 1, lightDiffuse2);
		glUniform3fv(LightaUniformPF[2], 1, lightAmbient2);
		glUniform3fv(LightsUniformPF[2], 1, lightSpecular2);
		glUniform4fv(LightpUniformPF[2], 1, lightPosition2);

		glUniform3fv(KdUniformPF, 1, materialDiffuse);
		glUniform3fv(KaUniformPF, 1, materialAmbient);
		glUniform3fv(KsUniformPF, 1, materialSpecular);
		glUniform1f(MaterialShineUniformPF, 128.0f);
    }
    else
    {
        glUniform1i(lKeyPressedUniformPF, 0);
    }
    
    glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// stop using OpenGL program object
	glUseProgram(0);

	// Animation
    
    glXSwapBuffers(gpDisplay, gWindow);
}

void UnInitialize(void)
{
	GLXContext currentGLXContext;

	if(gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}

	if(gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}

	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}

	currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}

		// detach vertex shader from shader program object
	glDetachShader(gShaderProgramObjectPF, gVertexShaderObjectPF);

	// detach fragment shader from shader program object
	glDetachShader(gShaderProgramObjectPF, gFragmentShaderObjectPF);

	// delete vertex shader object
	glDeleteShader(gVertexShaderObjectPF);
	gVertexShaderObjectPF = 0;

	// delete fragment shader object
	glDeleteShader(gFragmentShaderObjectPF);
	gFragmentShaderObjectPF = 0;

	// delete shader program object
	glDeleteProgram(gShaderProgramObjectPF);
	gShaderProgramObjectPF = 0;

	// unlink shader program
	glUseProgram(0);
}
