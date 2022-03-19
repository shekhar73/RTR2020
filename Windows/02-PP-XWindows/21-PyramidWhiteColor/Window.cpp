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

int gVertexShaderObject;
int gFragmentShaderObject;
int gShaderProgramObject;

unsigned int gVao_Pyramid;
unsigned int gVbo_Position_Pyramid;
unsigned int gVbo_Color_Pyramid;

unsigned int gMVPUniform;

float pyramidAngle = 0.0f;

mat4 perspectiveProjectionMatrix;

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
	static int frameBufferAttributes[] = {GLX_RGBA, 
											GLX_RED_SIZE, 1,
											GLX_GREEN_SIZE, 1,
											GLX_BLUE_SIZE, 1,
											GLX_ALPHA_SIZE, 1,
											0}; 

	
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR : Unable To Open X Display.\nExitting Now...\n");
		UnInitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);

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

	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
		gGLXContext = NULL;
	}

	// vertex shader
	// create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * vPosition;" \
		"out_color = vColor;" \
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
		"#version 450 core"	\
		"\n"	\
		"in vec4 out_color;"	\
		"out vec4 FragColor;"	\
		"void main(void)"	\
		"\n"	\
		"{"		\
		"FragColor = out_color;"
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
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");

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
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

	// vertices, colors, shader attribs, vbo, vao, initializations
	const GLfloat pyramidVertices[] = {

									// front side
									0.0f, 0.5f, 0.0f,
									-0.5f, -0.5f, 0.5f,
									0.5f, -0.5f, 0.5f,

									// left side
									0.0f, 0.5f, 0.0f,
									-0.5f, -0.5f, -0.5f,
									-0.5f, -0.5f, 0.5f,

									// back side
									0.0f, 0.5f, 0.0f,
									0.5f, -0.5f, -0.5f,
									-0.5f, -0.5f, -0.5f,

									// right side
									0.0f, 0.5f, 0.0f,
									0.5f, -0.5f, 0.5f,
									0.5f, -0.5f, -0.5f
	};

	const GLfloat colorCube[] = { 1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f 
	};

	const GLfloat colorPyramid[] = { 1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,

									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &gVao_Pyramid);
	glBindVertexArray(gVao_Pyramid);

	glGenBuffers(1, &gVbo_Position_Pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Color_Pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorPyramid), colorPyramid, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


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
	glClear(GL_COLOR_BUFFER_BIT);

	// start using opengl program object
	glUseProgram(gShaderProgramObject);

	// opengl drawing
	// set modelview & modelviewprojection matrix to identity
	mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	mat4 RotationMatrix = vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);
	mat4 modelViewMatrix = TranslateMatrix * RotationMatrix;
	mat4 modelViewProjectionMatrix = mat4::identity();

	// multiply modelview and perspective matrix to get modelviewprojection matrix
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // order is important

	//pass above modelviewprojection matrix to the vertex shader in 'u_mvpMatrix' shader variable
	// whose position value we already calculated in intiWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

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


	glFlush();
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
