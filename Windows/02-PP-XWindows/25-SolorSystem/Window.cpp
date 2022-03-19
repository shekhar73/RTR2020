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

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

unsigned int gVao_sphere;
unsigned int gVbo_sphere_position;
unsigned int gVbo_sphere_normal;
unsigned int gVbo_sphere_element;

unsigned int gNumVertices;
unsigned int gNumElements;

unsigned int gModelMatrix;
unsigned int gViewMatrix;
unsigned int gProjectionMatrix;

// Light
unsigned int LaUniform;
unsigned int LdUniform;
unsigned int LsUniform;


// Material
unsigned int KaUniform;
unsigned int KdUniform;
unsigned int KsUniform;

unsigned int lightPositionUniform;;
float materialShineness;
unsigned int LKeyPressedUniform;

bool bLight;

int day = 0;
int year = 0;
int earth = 0;

mat4 MATRIXSTACK[MAX];
int TOP = -1;

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

					case 0x44:
					day = (day + 6) % 360;
					break;

				case 0x64:
					day = (day - 6) % 360;
					break;

				case 0x45:
					earth = (earth + 3) % 360;
					break;

				case 0x65:
					earth = (earth - 3) % 360;
					break;

				case 0x59:
					year = (year + 3) % 360;
					break;

				case 0x79:
					year = (year - 3) % 360;
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

	// Provide source code to shader
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform int u_l_key_preesed;" \
		"uniform vec4 u_light_position;" \
		"out vec3 light_direction;" \
		"out vec3 view_vector;" \
		"out vec3 transformed_normal;" \
		"void main(void)" \
		"{" \
		"if(u_l_key_preesed == 1)" \
		"{" \
		"vec4 eye_coordinates		= u_viewMatrix * u_modelMatrix * vPosition;" \
		"transformed_normal			= mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
		"light_direction			= vec3(u_light_position - eye_coordinates);" \
		"view_vector				= -eye_coordinates.xyz;" \
		"}"	\
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix  * vPosition;"  \
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

	// provide source code to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"out vec4 FragColor;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ks;" \
		"uniform int u_l_key_preesed;" \
		"in vec3 light_direction;" \
		"in vec3 view_vector;" \
		"in vec3 transformed_normal;" \
		"in vec3 reflection_vector;" \
		"uniform float u_shineness;" \
		"vec3 fong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_l_key_preesed == 1)" \
		"{" \
		"vec3 normalized_transformed_normals		= normalize(transformed_normal);" \
		"vec3 normalized_light_directions			= normalize(light_direction);" \
		"vec3 normalized_view_vectors				= normalize(view_vector);" \
		"vec3 reflection_vector					= reflect(-normalized_light_directions, normalized_transformed_normals);" \
		"vec3 ambient_light						= u_la * u_ka;" \
		"vec3 diffuse_light						= u_ld * u_kd * max(dot(normalized_light_directions, normalized_transformed_normals), 0.0f);" \
		"vec3 specular 							= u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vectors), 0.0f), u_shineness);" \
		"fong_ads_light							= ambient_light + diffuse_light + specular;" \
		"}"	\
		"else"	\
		"{" \
		"fong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"FragColor = vec4(fong_ads_light, 1.0f);" \
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
	gModelMatrix = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
	if (gModelMatrix == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gViewMatrix = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
	if (gViewMatrix == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	gProjectionMatrix = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");
	if (gProjectionMatrix == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
	if (LaUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
	if (LdUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
	if (LsUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	if (KaUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	if (KdUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
	if (KsUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}


	materialShineness = glGetUniformLocation(gShaderProgramObject, "u_shineness");
	if (materialShineness == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	if (lightPositionUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_l_key_preesed");
	if (LKeyPressedUniform == -1) {
		fprintf(gpFile, "%d\n", __LINE__);
	}
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// It will store all the below 6 steps so that we can reuse it in draw function
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// Postion vbo
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Unbind Vao
	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// blue

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
	
	// function declarations
	void pushMatrix(mat4);
	mat4 popMatrix(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };	//if w(4th parameter) = 0.0f then it is directional light or if w = 1.0f then it is positional light

	if (bLight == true)
	{
		// Light
		GLfloat LaUniforms[] = { 0.1f, 0.1f, 0.1f };
		GLfloat LdUniforms[] = { 1.0f, 1.0f, 1.0f };
		GLfloat LsUniforms[] = { 1.0f, 1.0f, 1.0f };

		// Material
		GLfloat KaUniforms[] = { 0.0f, 0.0f, 0.0f };
		GLfloat KdUniforms[] = { 0.5f, 0.2f, 0.7f };
		GLfloat KsUniforms[] = { 0.7f, 0.7f, 0.7f };

		glUniform1i(LKeyPressedUniform, 1);
		glUniform3fv(LaUniform, 1, (GLfloat*)LaUniforms);
		glUniform3fv(LdUniform, 1, (GLfloat*)LdUniforms);
		glUniform3fv(LsUniform, 1, (GLfloat*)LsUniforms);

		glUniform3fv(KaUniform, 1, (GLfloat*)KaUniforms);
		glUniform3fv(KdUniform, 1, (GLfloat*)KdUniforms);
		glUniform3fv(KsUniform, 1, (GLfloat*)KsUniforms);

		glUniform1f(materialShineness, 128.0f);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);		// It sends vector of 4 floats - sending light in an array - to send in array format use this glUniform4fv() function.

	// OpenGL Drawing - set modelview & modelviewprojection matrices to identity
	mat4 ModelMatrix = mat4::identity();
	mat4 ViewMatrix = mat4::identity();
	mat4 ProjectionMatrix = mat4::identity();
	mat4 translateMatrix = mat4::identity();
	translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

	ModelMatrix = translateMatrix;

	glUniformMatrix4fv(gModelMatrix, 1, GL_FALSE, ModelMatrix);
	glUniformMatrix4fv(gViewMatrix, 1, GL_FALSE, ViewMatrix);
	glUniformMatrix4fv(gProjectionMatrix, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	pushMatrix(ModelMatrix);
		glBindVertexArray(gVao_sphere);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
			glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);

	ModelMatrix = popMatrix();

	
	
	
	ModelMatrix = mat4::identity();
	ViewMatrix = mat4::identity();
	ProjectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	mat4 rotationMatrixY = mat4::identity();
	mat4 rotationMatrixZ = mat4::identity();

	pushMatrix(ModelMatrix);
		translateMatrix = vmath::translate(2.5f, 0.0f, -10.0f);


		rotationMatrixY = vmath::rotate((GLfloat)year, 0.0f, 1.0f, 0.0f);
		ModelMatrix = ModelMatrix * rotationMatrixY * translateMatrix;
		rotationMatrixZ = vmath::rotate((GLfloat)day, 0.0f, 1.0f, 0.0f);

		glUniformMatrix4fv(gModelMatrix, 1, GL_FALSE, ModelMatrix);
		glUniformMatrix4fv(gViewMatrix, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(gProjectionMatrix, 1, GL_FALSE, gPerspectiveProjectionMatrix);

		pushMatrix(ModelMatrix);
			glBindVertexArray(gVao_sphere);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
				glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);
		ModelMatrix = popMatrix();
	ModelMatrix = popMatrix();

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
