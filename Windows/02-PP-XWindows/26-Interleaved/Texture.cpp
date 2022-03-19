#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/glew.h>

#include<GL/gl.h> // for OPenGL
#include<GL/glx.h>	// for glx API

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include"vmath.h"


using namespace std;
using namespace vmath;

enum
{
	SSK_ATTRIBUTE_VERTEX = 0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
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

int gVertexShaderObject;
int gFragmentShaderObject;
int gShaderProgramObject;


unsigned int marbleTexture;
unsigned int textureSamplerUniform;

unsigned int gVaoCube;
unsigned int gVboPCNTCube;

unsigned int modelViewMatrixUniform;
unsigned int perspectiveProjectionUniform;
unsigned int LKeyPressedUniform;
unsigned int LdUniform;
unsigned int KdUniform;
unsigned int lightPositionUniform;


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

					case XK_F:
					case XK_f:
					if(bFullscreen == false)
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
						if (bLight == false)
							bLight = true;
						else
							bLight = false;
						break;
					}
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

	
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int styleMask;
	static int frameBufferAttributes[] = {GLX_DOUBLEBUFFER,
											True,
											GLX_RGBA, 
											GLX_RED_SIZE, 8,
											GLX_GREEN_SIZE, 8,
											GLX_BLUE_SIZE, 8,
											GLX_ALPHA_SIZE, 8,
											GLX_DEPTH_SIZE, 24,
											0}; 

	
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR : Unable To Open X Display.\nExitting Now...\n");
		UnInitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	pGLXFBConfig = glXChooseFBConfig(gpDisplay, defaultScreen, frameBufferAttributes, &numFBConfigs);
	printf("Found number of FBCOnfigs : %d\n", numFBConfigs);
	int bestFrameBufferConfig = -1;
	int worstFrameBufferConfig = -1;
	int bestNumberOfSamples = -1;
	int worstNumberOfSamples = 999;
	for (int i = 0; i < numFBConfigs; i++)
	{
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
		if (pTempXVisualInfo != NULL)
		{
			int sampleBuffers, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);

			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);

			if (bestFrameBufferConfig < 0 || sampleBuffers && samples>bestNumberOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}

			if (worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
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
	if (gpXVisualInfo == NULL) 
	{
		printf("Error : Unable to get a Visual.\nExiting Now..\n");
		uninitialize();
		exit(1);
	}

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay,
							RootWindow(gpDisplay, gpXVisualInfo->screen),
							gpXVisualInfo->visual,
							AllocNone);

	gColormap = winAttribs.colormap;

	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);

	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow(gpDisplay,
				RootWindow(gpDisplay, gpXVisualInfo->screen),
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
		printf("ERROR : Failed To Create Main Window.\nExitting Now...\n");
        UnInitialize();
        exit(1);
	}

	XStoreName(gpDisplay, gWindow, "SSK");

	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);

	XMapWindow(gpDisplay, gWindow);
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
	unsigned int loadBMPAsTexture(const char*);

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
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXTURE0, "vTexCoord");

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
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	
	STONE_TEXTURE = loadBMPAsTexture("Stone.bmp");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	Resize(giWindowWidth, giWindowHeight);
}

unsigned int loadBMPAsTexture(const char* path) 
{
	// Variable declarations
	int width, height;
	unsigned char* imageData = NULL;
	unsigned int textureID;
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);

	// OpenGL Code
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(imageData);

	return textureID;
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
