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


unsigned int vao;
unsigned int vbo_position;
unsigned int vbo_normal;
unsigned int vbo_element;

unsigned int modelMatrixUniform;
unsigned int perspectiveProjectionUniform;
unsigned int viewMatrixUniform;

unsigned int LaUniform;	// Light Ambient
unsigned int LsUniform;	// Light Specular
unsigned int LdUniform;	// Light Diffuse
unsigned int LpUniform;	// Light Position

unsigned int KaUniform;	// Material Ambient
unsigned int KdUniform;	// Diffuse
unsigned int KsUniform;	// Specular
float MaterialShineUniform;

unsigned int lKeyPressedUniform;

bool bAnimate = false;
bool bLight = false;

mat4 MATRIXSTACK[MAX];
int TOP = -1;
float shoulder = 0.0f;
float elbow = 0.0f;

unsigned int gVao_sphere;

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


				case 'S':
				case 's':
					shoulder = shoulder + 0.5f;
					break;

				case 'E':			
				case 'e':
					elbow = elbow + 0.5f;
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
	const GLchar *vertexShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;"	\
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;"	\
		"uniform mat4 u_projectionMatrix;"	\
		"uniform int u_lKeyPressed;"	\
		"uniform vec4 light_position;"	\
		"out vec3 transformed_Normal;"	\
		"out vec3 light_direction;"	\
		"out vec3 view_vector;"	\
		"void main(void)" \
		"\n" \
		"{" \
		"if(u_lKeyPressed == 1)"	\
		"{"	\
		"vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;"	\
		"transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;"	\
		"light_direction = vec3(light_position - eyeCoordinate);"	\
		"view_vector = -eyeCoordinate.xyz;"	\
		"}"	\
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
	const GLchar *fragmentShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"vec3 phong_ads_light;"	\
		"in vec3 transformed_Normal;"	\
		"in vec3 light_direction;"	\
		"in vec3 view_vector;"	\
		"out vec4 FragColor;"	\
		"uniform vec3 u_ld;"	\
		"uniform vec3 u_la;"	\
		"uniform vec3 u_ls;"	\
		"uniform vec4 light_position;"	\
		"uniform vec3 u_kd;"	\
		"uniform vec3 u_ka;"	\
		"uniform vec3 u_ks;"	\
		"uniform float materialShineUniform;"	\
		"uniform int u_lKeyPressed;"	\
		"void main(void)" \
		"\n" \
		"{" \
		"if(u_lKeyPressed == 1)"	\
		"{"	\
			"vec3 normalizedTransformNormal = normalize(transformed_Normal);"	\
			"vec3 normalizedLightDirection = normalize(light_direction);"	\
			"vec3 normalizedViewVector = normalize(view_vector);"	\
			"vec3 ambient = u_la * u_ka;"	\
			"vec3 diffuse = u_ld * u_kd * max(dot(normalizedLightDirection, normalizedTransformNormal), 0.0f);"	\
			"vec3 reflection_vector = reflect(-normalizedLightDirection, normalizedTransformNormal);"	\
			"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalizedViewVector), 0.0f), materialShineUniform);"	\
			"phong_ads_light = ambient + diffuse + specular;"	\
		"}"	\
		"else"	\
		"{"	\
			"phong_ads_light = vec3(1.0, 1.0, 1.0);"	\
		"}"	\
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
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vPosition");
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
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");

	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");

	lKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_lKeyPressed");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
	LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
	LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
	LpUniform = glGetUniformLocation(gShaderProgramObject, "light_position");

	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
	MaterialShineUniform = glGetUniformLocation(gShaderProgramObject, "materialShineUniform");


	// vertices, colors, shader attribs, vbo, vao, initializations
	const float triangleVertices[] = { 
											0.0f, 	1.0f, 	0.0f,
											-1.0f, 	-1.0f, 	0.0f,
											1.0f, 	-1.0f, 	0.0f
										};

	getSphereVertexData(sphere_vertices, shpere_normals, shpere_textures, sphere_elements);
	gNumSphereVertices = getNumberOfSphereVertices();
	gNumSphereElements = getNumberOfSphereElements();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Normal
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shpere_normals), shpere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Element drawing
	glGenBuffers(1, &vbo_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	Resize(giWindowWidth, giWindowHeight);
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// function declarations
	void pushMatrix(mat4);
	mat4 popMatrix(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { 120.0f, 120.0f, 100.0f, 1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat materialAmbient[] = { 1.5f, 1.5f, 0.5f, 1.0f };
	GLfloat materialDiffuse[] = { 1.7f, 1.7f, 0.7f, 1.0f };
	GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
	GLfloat materialShine = 128.0f;

	if (bLight == true)
	{
		glUniform1i(lKeyPressedUniform, 1);

		glUniform3fv(LdUniform, 1, lightDiffuse);
		glUniform3fv(LaUniform, 1, lightAmbient);
		glUniform3fv(LsUniform, 1, lightSpecular);
		glUniform4fv(LpUniform, 1, lightPosition);

		glUniform3fv(KdUniform, 1, materialDiffuse);
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KsUniform, 1, materialSpecular);
		glUniform1f(MaterialShineUniform, materialShine);
	}
	else
	{
		glUniform1i(lKeyPressedUniform, 0);
	}

	// OpenGL Drawing
	// set modelview & modelviewprojection matrices to identity
	mat4 translateMatrix = mat4::identity();
	mat4 rotationMatrixX = mat4::identity();
	mat4 rotationMatrixY = mat4::identity();
	mat4 rotationMatrixZ = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	translateMatrix = vmath::translate(0.0f, 0.0f, -12.0f);
	modelMatrix = translateMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

	pushMatrix(modelMatrix);
		rotationMatrixZ = vmath::rotate(shoulder, 0.0f, 0.0f, 1.0f);
		translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
		modelMatrix = modelMatrix * rotationMatrixZ * translateMatrix;

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		pushMatrix(modelMatrix);
			scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
			modelMatrix = modelMatrix * scaleMatrix;
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

			glBindVertexArray(vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
				glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);
	modelMatrix = popMatrix();



	translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
	rotationMatrixZ = vmath::rotate(elbow, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * translateMatrix * rotationMatrixZ;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * translateMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	pushMatrix(modelMatrix);
	scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
	modelMatrix = modelMatrix * scaleMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	modelMatrix = popMatrix();

	modelMatrix = popMatrix();

	// Stop using OpenGL program object
	glUseProgram(0);

	glFlush();
}

void pushMatrix(mat4 matrix)
{
	if (TOP < MAX - 1)
	{
		TOP++;
		MATRIXSTACK[TOP] = matrix;
	}
	else
	{

	}
}

mat4 popMatrix()
{
	if (TOP > 0)
	{
		int temp = TOP;
		TOP--;
		return MATRIXSTACK[temp];
	}
	else
	{
		return 0;
	}
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
