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

unsigned int gVao_Triangle;
unsigned int gVbo_Triangle_Position;
unsigned int gVbo_Triangle_Texture;

unsigned int gVao_Square;
unsigned int gVbo_Square_Position;
unsigned int gVbo_Square_Texture;

unsigned int gMVPUniform;
unsigned int textureSamplerUniform;

mat4 perspectiveProjectionMatrix;

FILE* gpFile = NULL;

GLuint STONE_TEXTURE;

GLfloat triangleAngle = 0.0f;
GLfloat squareAngle = 0.0f;


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
	GLuint loadBMPAsTexture(const char*);

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
		"in vec2 vTexture;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec2 out_texture;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texture = vTexture;" \
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
		"in vec2 out_texture;" \
		"uniform sampler2D u_texture_sampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_texture_sampler,out_texture);" \
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
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vTexture");

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
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	// vertices, colors, shader attribs, vbo, vao, initializations
	const GLfloat triangleVertices[] = { 
											0.0f, 0.5f, 0.0f,
											-0.5f, -0.5f, 0.5f,
											0.5f, -0.5f, 0.5f,

											0.0f, 0.5f, 0.0f,
											-0.5f, -0.5f, -0.5f,
											-0.5f, -0.5f, 0.5f,

											0.0f, 0.5f, 0.0f,
											0.5f, -0.5f, -0.5f,
											-0.5f, -0.5f, -0.5f,

											0.0f, 0.5f, 0.0f,
											0.5f, -0.5f, 0.5f,
											0.5f, -0.5f, -0.5f 
										};

	const GLfloat pyramidTextures[] = 
									{
										// Front
										0.5f, 0.0f,
										0.0f, 1.0f,
										1.0f, 1.0f,
										// Right
										0.5f, 0.0f,
										0.0f, 1.0f,
										1.0f, 1.0f,
										// Back
										0.5f, 0.0f,
										0.0f, 1.0f,
										1.0f, 1.0f,
										// Left
										0.5f, 0.0f,
										0.0f, 1.0f,
										1.0f, 1.0f
									};

	const GLfloat squareVertices[] = {	
										0.5f, 0.5f, 0.5f,
										-0.5f, 0.5f, 0.5f,
										-0.5f, -0.5f, 0.5f,
										0.5f, -0.5f, 0.5f,

										-0.5f, 0.5f, -0.5f,
										0.5f, 0.5f, -0.5f,
										0.5f, -0.5f, -0.5f,
										-0.5f, -0.5f, -0.5f,

										0.5f, 0.5f, -0.5f,
										0.5f, 0.5f, 0.5f,
										0.5f, -0.5f, 0.5f,
										0.5f, -0.5f, -0.5f,

										-0.5f, 0.5f, 0.5f,
										-0.5f, 0.5f, -0.5f,
										-0.5f, -0.5f, -0.5f,
										-0.5f, -0.5f, 0.5f,

										0.5f, 0.5f, -0.5f,
										-0.5f, 0.5f, -0.5f,
										-0.5f, 0.5f, 0.5f,
										0.5f, 0.5f, 0.5f,

										-0.5f, -0.5f, -0.5f,
										0.5f, -0.5f, -0.5f,
										0.5f, -0.5f, 0.5f,
										-0.5f, -0.5f, 0.5f
									};

	const GLfloat cubeTextures[] = 
								{
									// Front
									1.0f, 0.0f,
									0.0f, 0.0f,
									0.0f, 1.0f,
									1.0f, 1.0f,
									// Right
									1.0f, 0.0f,
									0.0f, 0.0f,
									0.0f, 1.0f,
									1.0f, 1.0f,
									// Back
									1.0f, 0.0f,
									0.0f, 0.0f,
									0.0f, 1.0f,
									1.0f, 1.0f,
									// Left
									1.0f, 0.0f,
									0.0f, 0.0f,
									0.0f, 1.0f,
									1.0f, 1.0f,
									// Top
									1.0f, 0.0f,
									0.0f, 0.0f,
									0.0f, 1.0f,
									1.0f, 1.0f,
									// Bottom
									1.0f, 0.0f,
									0.0f, 0.0f,
									0.0f, 1.0f,
									1.0f, 1.0f,
								};

	glGenVertexArrays(1, &gVao_Triangle);
	glBindVertexArray(gVao_Triangle);

	glGenBuffers(1, &gVbo_Triangle_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Triangle_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Triangle_Texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Triangle_Texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTextures), pyramidTextures, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &gVao_Square);
	glBindVertexArray(gVao_Square);

	glGenBuffers(1, &gVbo_Square_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Square_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Square_Texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Square_Texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextures), cubeTextures, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	
	STONE_TEXTURE = loadBMPAsTexture("Stone.bmp");
	KUNDALI_TEXTURE = loadBMPAsTexture("Kundali.bmp");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	Resize(giWindowWidth, giWindowHeight);
}

GLuint loadBMPAsTexture(const char* path) 
{
	// Variable declarations
	int width, height;
	unsigned char* imageData = NULL;
	GLuint textureID;
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
	// Variables
	static GLfloat rotationX = 0.0f;
	static GLfloat rotationY = 0.0f;
	static GLfloat rotationZ = 0.0f;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	mat4 translateMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationXMatrix = mat4::identity();
	mat4 rotationYMatrix = mat4::identity();
	mat4 rotationZMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();
	translateMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
	rotationYMatrix = vmath::rotate(rotationY, 0.0f, 1.0f, 0.0f);
	modelViewMatrix = translateMatrix * rotationYMatrix;
	
	modelViewProjectionMatrix = perspectiveProjectMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, STONE_TEXTURE);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_pyramid);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);

	
	translateMatrix = mat4::identity();
	rotationXMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translateMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
	scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
	rotationXMatrix = vmath::rotate(rotationX, 1.0f, 0.0f, 0.0f);
	rotationYMatrix = vmath::rotate(rotationY, 0.0f, 1.0f, 0.0f);
	rotationZMatrix = vmath::rotate(rotationZ, 0.0f, 0.0f, 1.0f);
	modelViewMatrix = translateMatrix * scaleMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix;
	
	modelViewProjectionMatrix = perspectiveProjectMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, KUNDALI_TEXTURE);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
	glUseProgram(0);

	rotationX += 1.0f;
	rotationY += 1.0f;
	rotationZ += 1.0f;

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

	// destroy vao
	if (gVao_Triangle)
	{
		glDeleteVertexArrays(1, &gVao_Triangle);
		gVao_Triangle = 0;
	}

	// destroy vbo
	if (gVbo_Triangle_Position)
	{
		glDeleteVertexArrays(1, &gVbo_Triangle_Position);
		gVbo_Triangle_Position = 0;
	}

	if (gVbo_Triangle_Texture)
	{
		glDeleteBuffers(1, &gVbo_Triangle_Texture);
		gVbo_Triangle_Texture = 0;
	}

	if (gVao_Square)
	{
		glDeleteVertexArrays(1, &gVao_Square);
		gVao_Square = 0;
	}

	// destroy vbo
	if (gVbo_Square_Position)
	{
		glDeleteVertexArrays(1, &gVbo_Square_Position);
		gVbo_Square_Position = 0;
	}

	if (gVbo_Square_Texture)
	{
		glDeleteBuffers(1, &gVbo_Square_Texture);
		gVbo_Square_Texture = 0;
	}
}
