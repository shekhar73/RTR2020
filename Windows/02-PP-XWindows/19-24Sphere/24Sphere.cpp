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

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,0.0f,0.0f,0.0f };

GLuint LightaUniformPF;
GLuint LightsUniformPF;
GLuint LightdUniformPF;
GLuint LightpUniformPF; 

GLuint KaUniformPF;
GLuint KdUniformPF;
GLuint KsUniformPF;
GLfloat MaterialShineUniformPF; 

GLuint lKeyPressedUniformPF;

GLfloat lightAngle0 = 0.0f;
GLfloat lightAngle1 = 0.0f;
GLfloat lightAngle2 = 0.0f;

bool bAnimate = false;
bool bLight = false;

mat4 perspectiveProjectionMatrix;

FILE* gpFile = NULL;

GLfloat lightRotateAngle = 0.0f;

int winWidth;
int winHeight;

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
	winHeight = giWindowHeight;
	winWidth = giWindowWidth;
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

				// Getting pressed keyboard alphabets by other way
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
					
					case 'X':
                    case 'x':
                        bAnimate = 1;
                        break;
                    case 'Y':
                    case 'y':
                        bAnimate = 2;
                        break;
                    case 'Z':
                    case 'z':
                        bAnimate = 3;
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
		"uniform vec4 u_lightp;" \
		"out vec3 transformed_Normal;" \
		"out vec3 light_direction;" \
		"out vec3 view_vector;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_lKeyPressed == 1){" \
		"vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
		"transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"light_direction = vec3(u_lightp - eyeCoordinate);" \
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
		"in vec3 light_direction;" \
		"in vec3 view_vector;" \
		"out vec4 FragColor;" \
		"uniform vec3 u_lightd;" \
		"uniform vec3 u_lighta;" \
		"uniform vec3 u_lights;" \
		"uniform vec4 u_lightp;" \
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
		"vec3 normalizedLightDirection = normalize(light_direction);" \
		"vec3 ambient = u_lighta * u_ka;" \
		"vec3 diffuse = u_lightd * u_kd * max(dot(normalizedLightDirection, normalizedTransformNormal) , 0.0f);" \
		"vec3 reflection_vector = reflect(-normalizedLightDirection, normalizedTransformNormal);" \
		"vec3 specular = u_lights * u_ks * pow(max(dot(reflection_vector, normalizedViewVector), 0.0f) ,  materialShineUniform);" \
		"phong_ads_light = phong_ads_light + ambient + diffuse + specular;" \
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

	
	LightdUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_lightd");
	LightaUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_lighta");
	LightsUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_lights");
	LightpUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_lightp");

	KdUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_kd");
	KaUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ka");
	KsUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ks");
	MaterialShineUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "materialShineUniform");
	 
    // Sphere Data
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumSphereVertices = getNumberOfSphereVertices();
	gNumSphereElements = getNumberOfSphereElements();
    
    
    // Circle
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
    
    bAnimate = 1;
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
 	mat4 translateMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationXMatrix = mat4::identity();
    mat4 rotationYMatrix = mat4::identity();
    mat4 rotationZMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

    // Object Material
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShine;
    
    
    
    // code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
	glUseProgram(gShaderProgramObjectPF);
    
    if(bLight == true){
        glUniform1i(lKeyPressedUniformPF, 1);
    }
    else{
        glUniform1i(lKeyPressedUniformPF, 0);
    }
    
	glUniform3fv(LightdUniformPF, 1, lightDiffuse);
	glUniform3fv(LightaUniformPF, 1, lightAmbient);
	glUniform3fv(LightsUniformPF, 1, lightSpecular);
	glUniform4fv(LightpUniformPF, 1, lightPosition);
	
	glUniformMatrix4fv(perspectiveProjectionUniformPF, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(viewMatrixUniformPF, 1, GL_FALSE, viewMatrix);

	// Sphere 1
	glViewport(0, (GLsizei)(winHeight/6) * 4, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 1 - Emrald 
	// Ambient
	materialAmbient[0] = 0.0215f;
	materialAmbient[1] = 0.1745f;
	materialAmbient[2] = 0.0215f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.07568f;
	materialDiffuse[1] = 0.61424f;
	materialDiffuse[2] = 0.07568f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.633f;
	materialSpecular[1] = 0.727811f;
	materialSpecular[2] = 0.633f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.6f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 2
	glViewport((GLsizei)(winWidth / 6), (GLsizei)(winHeight/6) * 4, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 2 - Jade
	// Ambient
	materialAmbient[0] = 0.135f;
	materialAmbient[1] = 0.2225f;
	materialAmbient[2] = 0.1575f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.54f;
	materialDiffuse[1] = 0.89f;
	materialDiffuse[2] = 0.63f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.316228f;
	materialSpecular[1] = 0.316228f;
	materialSpecular[2] = 0.316228f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.1f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 3
	glViewport((GLsizei)(winWidth / 6) * 2, (GLsizei)(winHeight/6) * 4, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 3 - Obsidian
	// Ambient
	materialAmbient[0] = 0.05375f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.06625f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.18275f;
	materialDiffuse[1] = 0.17f;
	materialDiffuse[2] = 0.22525f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.332741f;
	materialSpecular[1] = 0.328634f;
	materialSpecular[2] = 0.346435f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.3f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);


	// Sphere 4
	glViewport((GLsizei)(winWidth / 6) * 3, (GLsizei)(winHeight/6) * 4, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 4 - Pearl
	// Ambient
	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.20725f;
	materialAmbient[2] = 0.20725f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 1.0f;
	materialDiffuse[1] = 0.829f;
	materialDiffuse[2] = 0.829f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.296648f;
	materialSpecular[1] = 0.296648f;
	materialSpecular[2] = 0.296648f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.088f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 5
	glViewport((GLsizei)(winWidth / 6) * 4, (GLsizei)(winHeight/6) * 4, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 5 - Ruby
	// Ambient
	materialAmbient[0] = 0.1745f;
	materialAmbient[1] = 0.01175f;
	materialAmbient[2] = 0.01175f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.61424f;
	materialDiffuse[1] = 0.04136f;
	materialDiffuse[2] = 0.04136f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.727811f;
	materialSpecular[1] = 0.626959f;
	materialSpecular[2] = 0.626959f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.6f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 6
	glViewport((GLsizei)(winWidth / 6) * 5, (GLsizei)(winHeight/6) * 4, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	/// Sphere 6 - Turquoise
	// Ambient
	materialAmbient[0] = 0.1f;
	materialAmbient[1] = 0.18725f;
	materialAmbient[2] = 0.1745f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.396f;
	materialDiffuse[1] = 0.74151f;
	materialDiffuse[2] = 0.69102f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.297254f;
	materialSpecular[1] = 0.30829f;
	materialSpecular[2] = 0.306678f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.1f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Row 2
	// Sphere 7
	glViewport(0, (GLsizei)(winHeight/6) * 3, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 7 - Brass
	// Ambient
	materialAmbient[0] = 0.329412f;
	materialAmbient[1] = 0.223529f;
	materialAmbient[2] = 0.027451f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.780392f;
	materialDiffuse[1] = 0.568627f;
	materialDiffuse[2] = 0.113725f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.992157f;
	materialSpecular[1] = 0.941176f;
	materialSpecular[2] = 0.807843f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.21794872f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 8
	glViewport((GLsizei)(winWidth / 6), (GLsizei)(winHeight/6) * 3, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 8 - Bronze
	// Ambient
	materialAmbient[0] = 0.2125f;
	materialAmbient[1] = 0.1275f;
	materialAmbient[2] = 0.054f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.714f;
	materialDiffuse[1] = 0.4284f;
	materialDiffuse[2] = 0.18144f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.393548f;
	materialSpecular[1] = 0.271906f;
	materialSpecular[2] = 0.166721f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.2f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 9
	glViewport((GLsizei)(winWidth / 6) * 2, (GLsizei)(winHeight/6) * 3, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 9 - Chrome
	// Ambient
	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.25f;
	materialAmbient[2] = 0.25f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.4f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.774597f;
	materialSpecular[1] = 0.774597f;
	materialSpecular[2] = 0.774597f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.6f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);


	// Sphere 10
	glViewport((GLsizei)(winWidth / 6) * 3, (GLsizei)(winHeight/6) * 3, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 10 - Copper
	// Ambient
	materialAmbient[0] = 0.19125f;
	materialAmbient[1] = 0.0735f;
	materialAmbient[2] = 0.0225f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.7038f;
	materialDiffuse[1] = 0.27048f;
	materialDiffuse[2] = 0.0828f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.256777f;
	materialSpecular[1] = 0.137622f;
	materialSpecular[2] = 0.086014f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.1f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 11
	glViewport((GLsizei)(winWidth / 6) * 4, (GLsizei)(winHeight/6) * 3, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 11 - Gold
	// Ambient
	materialAmbient[0] = 0.24725f;
	materialAmbient[1] = 0.1995f;
	materialAmbient[2] = 0.0745f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.75164f;
	materialDiffuse[1] = 0.60648f;
	materialDiffuse[2] = 0.22648f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.628281f;
	materialSpecular[1] = 0.555802f;
	materialSpecular[2] = 0.366065f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.4f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 12
	glViewport((GLsizei)(winWidth / 6) * 5, (GLsizei)(winHeight/6) * 3, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 12 - Silver
	// Ambient
	materialAmbient[0] = 0.19225f;
	materialAmbient[1] = 0.19225f;
	materialAmbient[2] = 0.19225f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.50754f;
	materialDiffuse[1] = 0.50754f;
	materialDiffuse[2] = 0.50754f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.508273f;
	materialSpecular[1] = 0.508273f;
	materialSpecular[2] = 0.508273f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.4f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Row 3
	// Sphere 13
	glViewport(0, (GLsizei)(winHeight/6) * 2, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 13 - Black 
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.01f;
	materialDiffuse[1] = 0.01f;
	materialDiffuse[2] = 0.01f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.50f;
	materialSpecular[1] = 0.50f;
	materialSpecular[2] = 0.50f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.25f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 14
	glViewport((GLsizei)(winWidth / 6), (GLsizei)(winHeight/6) * 2, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 14 - Cyan 
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.1f;
	materialAmbient[2] = 0.06f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.0f;
	materialDiffuse[1] = 0.50980392f;
	materialDiffuse[2] = 0.50980392f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.50196078f;
	materialSpecular[1] = 0.50196078f;
	materialSpecular[2] = 0.50196078f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.25f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 15
	glViewport((GLsizei)(winWidth / 6) * 2, (GLsizei)(winHeight/6) * 2, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 15 - Green 
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.1f;
	materialDiffuse[1] = 0.35f;
	materialDiffuse[2] = 0.1f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.45f;
	materialSpecular[1] = 0.55f;
	materialSpecular[2] = 0.45f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.25f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);


	// Sphere 16
	glViewport((GLsizei)(winWidth / 6) * 3, (GLsizei)(winHeight/6) * 2, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 16 - Red
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.0f;
	materialDiffuse[2] = 0.0f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.6f;
	materialSpecular[2] = 0.6f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.25f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 17
	glViewport((GLsizei)(winWidth / 6) * 4, (GLsizei)(winHeight/6) * 2, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 17 - White
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.55f;
	materialDiffuse[1] = 0.55f;
	materialDiffuse[2] = 0.55f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.70f;
	materialSpecular[1] = 0.70f;
	materialSpecular[2] = 0.70f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.25f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 18
	glViewport((GLsizei)(winWidth / 6) * 5, (GLsizei)(winHeight/6) * 2, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 18 - Yellow Plastic | Row 4 - Coloumn 1 
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.0f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.60f;
	materialSpecular[1] = 0.60f;
	materialSpecular[2] = 0.50f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.25f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);


	// Row 4
	// Sphere 19
	glViewport(0, (GLsizei)(winHeight/6) * 1, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 19 - Black2
	// Ambient
	materialAmbient[0] = 0.02f;
	materialAmbient[1] = 0.02f;
	materialAmbient[2] = 0.02f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.01f;
	materialDiffuse[1] = 0.01f;
	materialDiffuse[2] = 0.01f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.4f;
	materialSpecular[1] = 0.4f;
	materialSpecular[2] = 0.4f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.078125f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 20
	glViewport((GLsizei)(winWidth / 6), (GLsizei)(winHeight/6) * 1, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 20 - Cyan2
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.04f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.7f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.078125f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 21
	glViewport((GLsizei)(winWidth / 6) * 2, (GLsizei)(winHeight/6) * 1, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 21 - Green2
	// Ambient
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.04f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.078125f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);


	// Sphere 22
	glViewport((GLsizei)(winWidth / 6) * 3, (GLsizei)(winHeight/6) * 1, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 22 - Red2
	// Ambient
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.4f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.04f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.078125f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 23
	glViewport((GLsizei)(winWidth / 6) * 4, (GLsizei)(winHeight/6) * 1, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 23 - White2
	// Ambient
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.7f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.078125f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	// Sphere 24
	glViewport((GLsizei)(winWidth / 6) * 5, (GLsizei)(winHeight/6) * 1, (GLsizei)(winWidth / 6), (GLsizei)(winHeight/6));
	translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translateMatrix;
	glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
	// Material 
	//------------------------------------------------
	// Sphere 24 - Yellow Rubber
	// Ambient
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// Diffuse
	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// Specular
	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	// Shine
	materialShine = 0.078125f * 128.0f;
	glUniform3fv(KdUniformPF, 1, materialDiffuse);
	glUniform3fv(KaUniformPF, 1, materialAmbient);
	glUniform3fv(KsUniformPF, 1, materialSpecular);
	glUniform1f(MaterialShineUniformPF, materialShine );
	// Object
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	
	if (bAnimate == 1) {
		lightPosition[0] = 0.0f;
		lightPosition[1] = 7.0f * sin(lightRotateAngle);
		lightPosition[2] = 7.0f * cos(lightRotateAngle);
		lightPosition[3] = 0.0f;

		lightRotateAngle = lightRotateAngle + 0.01f;
	}
	else if (bAnimate == 2) {
		lightPosition[0] = 7.0f * sin(lightRotateAngle);
		lightPosition[1] = 0.0f;
		lightPosition[2] = 7.0f * cos(lightRotateAngle);
		lightPosition[3] = 0.0f;

		lightRotateAngle = lightRotateAngle + 0.01f;
	}
	else if (bAnimate == 3){
		lightPosition[0] = 7.0f * sin(lightRotateAngle);
		lightPosition[1] = 7.0f * cos(lightRotateAngle);
		lightPosition[2] = 0.0f;
		lightPosition[3] = 0.0f;

		lightRotateAngle = lightRotateAngle + 0.01f;
	}
    
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

	if (vao) 
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_position) 
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vbo_normal) 
	{
		glDeleteBuffers(1, &vbo_normal);
		vbo_normal = 0;
	}

	if (vbo_element) 
	{
		glDeleteBuffers(1, &vbo_element);
		vbo_element = 0;
	}
}
