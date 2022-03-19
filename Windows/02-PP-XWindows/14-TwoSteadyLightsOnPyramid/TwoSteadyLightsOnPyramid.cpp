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


using namespace std;
using namespace vmath;

enum
{
	SSK_ATTRIBUTE_POSITION = 0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXCOORD,
};


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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint vao_pyramid;
GLuint vbo_pyramid_vertex;
GLuint vbo_pyramid_normal;

GLuint modelMatrixUniformPF;
GLuint perspectiveProjectionUniformPF;
GLuint viewMatrixUniformPF;


GLuint LightaUniformPF[2];
GLuint LightsUniformPF[2];
GLuint LightdUniformPF[2];
GLuint LightpUniformPF[2];

GLuint KaUniformPF;
GLuint KdUniformPF;
GLuint KsUniformPF;
GLfloat MaterialShineUniformPF;

GLuint lKeyPressedUniformPF;

bool bAnimate;
bool bLight = false;

mat4 perspectiveProjectionMatrix;

GLfloat rotationAngleY = 0.0f;

FILE* gpFile = NULL;


int main()
{
	
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void Initialize(void);
	void Resize(int, int);
	void Draw(void);
	void UnInitialize();

	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;
	bool bDone = false;
	
	CreateWindow();
	Initialize();

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

    s
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
	
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {0};

	
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

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	const int attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	if (!gGLXContext) {
		
		const int attribsLow[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};
		gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribsLow);
		printf("gGLX Context Low\n");
	}

	Bool isDirectContext = glXIsDirect(gpDisplay, gGLXContext);
	if (isDirectContext) {
		printf("Direct Hardware Rendering Context obtained\n");
	}
	else {
		printf("Software Rendering Context obtained\n");
	}

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	printf("OpenGL Vendor		:%s\n", glGetString(GL_VENDOR));
	printf("OpenGL Renderer	:%s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version		:%s\n", glGetString(GL_VERSION));
	printf("GLSL Version		:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLenum err = glewInit();

	// vertex shader
	// create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform int u_lKeyPressed;" \
		"uniform vec4 u_lightp[2];" \
		"out vec3 transformed_Normal;" \
		"out vec3 light_direction[2];" \
		"out vec3 view_vector;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_lKeyPressed == 1){" \
		"vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
		"transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"for (int i = 0; i < 2; i++) { "\
		"light_direction[i] = vec3(u_lightp[i] - eyeCoordinate);" \
		"}" \
		"view_vector = -eyeCoordinate.xyz;" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
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

	// fragment shader
	// create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide sourcecode to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"vec3 phong_ads_light;" \
		"in vec3 transformed_Normal;" \
		"in vec3 light_direction[2];" \
		"in vec3 view_vector;" \
		"out vec4 FragColor;" \
		"uniform vec3 u_lightd[2];" \
		"uniform vec3 u_lighta[2];" \
		"uniform vec3 u_lights[2];" \
		"uniform vec4 u_lightp[2];" \
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
		"for (int i = 0; i < 2; i++) { " \
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

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile shader
	glCompileShader(gFragmentShaderObject);

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
	// create
	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

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

	// get MVP uniform location
	modelMatrixUniformPF = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	viewMatrixUniformPF = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
	perspectiveProjectionUniformPF = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");
	lKeyPressedUniformPF = glGetUniformLocation(gShaderProgramObject, "u_lKeyPressed");

	LightdUniformPF[0] = glGetUniformLocation(gShaderProgramObject, "u_lightd[0]");
	LightaUniformPF[0] = glGetUniformLocation(gShaderProgramObject, "u_lighta[0]");
	LightsUniformPF[0] = glGetUniformLocation(gShaderProgramObject, "u_lights[0]");
	LightpUniformPF[0] = glGetUniformLocation(gShaderProgramObject, "u_lightp[0]");

	LightdUniformPF[1] = glGetUniformLocation(gShaderProgramObject, "u_lightd[1]");
	LightaUniformPF[1] = glGetUniformLocation(gShaderProgramObject, "u_lighta[1]");
	LightsUniformPF[1] = glGetUniformLocation(gShaderProgramObject, "u_lights[1]");
	LightpUniformPF[1] = glGetUniformLocation(gShaderProgramObject, "u_lightp[1]");

	KdUniformPF = glGetUniformLocation(gShaderProgramObject, "u_kd");
	KaUniformPF = glGetUniformLocation(gShaderProgramObject, "u_ka");
	KsUniformPF = glGetUniformLocation(gShaderProgramObject, "u_ks");
	MaterialShineUniformPF = glGetUniformLocation(gShaderProgramObject, "materialShineUniform");

	const GLfloat pyramidVertices[] =
	{
		//Front
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,
		//Right
		0.0f,1.0f,0.0f,
		1.0f,-1.0f,1.0f,
		1.0f,-1.0f,-1.0f,
		//Back
		0.0f,1.0f,0.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		//left
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f
	};

	const GLfloat pyramidNormals[] = 
	{
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,

		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,

		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f

	};

    
    // Rectangle
    glGenVertexArrays(1, &vao_pyramid);
	glBindVertexArray(vao_pyramid);
    
    glGenBuffers(1, &vbo_pyramid_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Light
	glGenBuffers(1, &vbo_pyramid_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal);
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
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    perspectiveProjectionMatrix = mat4::identity();
    
    Resize(giWindowWidth, giWindowHeight);
    
    bAnimate = false;
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    
	GLfloat lightAmbient0[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat lightDiffuse0[] = { 1.0f,0.0f,0.0f,1.0f };
	GLfloat lightSpecular0[] = { 1.0f,0.0f,0.0f,1.0f };
	GLfloat lightPosition0[] = { 2.0f,0.0f,0.0f,1.0f };

	GLfloat lightAmbient1[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat lightDiffuse1[] = { 0.0f,0.0f,1.0f,1.0f };
	GLfloat lightSpecular1[] = { 0.0f,0.0f,1.0f,1.0f };
	GLfloat lightPosition1[] = { -2.0f,0.0f,0.0f,1.0f };

	GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat materialShine = 50.0f;
      
	glUseProgram(gShaderProgramObject);
    

	
	translateMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
    rotationYMatrix = vmath::rotate(rotationAngleY, 0.0f, 1.0f, 0.0f);
	modelMatrix = translateMatrix * rotationYMatrix;
	
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

		glUniform3fv(KdUniformPF, 1, materialDiffuse);
		glUniform3fv(KaUniformPF, 1, materialAmbient);
		glUniform3fv(KsUniformPF, 1, materialSpecular);
		glUniform1f(MaterialShineUniformPF, 128.0f);
    }
    else
    {
        glUniform1i(lKeyPressedUniformPF, 0);
    }

	glBindVertexArray(vao_pyramid);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);


	// stop using OpenGL program object
	glUseProgram(0);

	
	if (bAnimate == true) 
	{
		rotationAngleY += 0.5f;
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
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	// unlink shader program
	glUseProgram(0);
}
