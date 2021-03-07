#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

bool bLight = false;
bool bTexture = false;

GLfloat lightAmbientZero[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuseZero[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPositionZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis It is a Positional Light
GLfloat lightSpecularZero[] = { 1.0f, 0.0f, 0.0f, 1.0f };

GLfloat lightAmbientOne[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuseOne[]  = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat lightPositionOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis It is a Positional Light
GLfloat lightSpecularOne[] = { 0.0f, 1.0f, 0.0f, 1.0f };

GLfloat lightAmbientTwo[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuseTwo[]  = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat lightPositionTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis It is a Positional Light
GLfloat lightSpecularTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };

GLfloat materialAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };		// Black
GLfloat materialDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat materialShineness = 100.0f;

GLfloat lightAngle0 = 0.0f;
GLfloat lightAngle1 = 0.0f;
GLfloat lightAngle2 = 0.0f;
//namespaces
using namespace std;

//global variable declarations
bool bFullScreen			= false;
Display *gpDisplay			= NULL;
XVisualInfo *gpXVisualInfo	= NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;
GLuint stoneTexture;

int giWindowWidth			= 800;
int giWindowHeight			= 600;

GLUquadric* quadric = NULL;
// entry-point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullScreen(void);
	void Initialize(void);
	void uninitialize(void);
	void Draw();
	void Resize(int, int);
	//variable declarations
	int winWidth 	= giWindowWidth;
	int winHeight 	= giWindowHeight;
	bool bDone		= false;

	//code
	CreateWindow();

	Initialize();

	//Message Loop
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
					uninitialize();
					exit(0);

					case XK_F:
					case XK_f:
					if(bFullScreen == false)
					{
						ToggleFullScreen();
						bFullScreen = true;
					}
					else
					{
						ToggleFullScreen();
						bFullScreen = false;
					}
					break;

					case 'L':
					case 'l':
					if(bLight == false)
					{
						glEnable(GL_LIGHTING);
						bLight = true;
					}
					else
					{
						glDisable(GL_LIGHTING);
						bLight = false;
					}
					break;

					default:
					break;

				}
				break;

				case MotionNotify:
				break;

				case ConfigureNotify:
				winWidth 	= event.xconfigure.width;
				winHeight 	= event.xconfigure.height;
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
	uninitialize();
	return (0);
}

void CreateWindow(void)
{

	void uninitialize(void);

	//variable declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
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

	//code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR : Unable to open X Display.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}

	defaultScreen 	= XDefaultScreen(gpDisplay);
	//defaultDepth 	= DefaultDepth(gpDisplay, defaultScreen);
	gpXVisualInfo 	= glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);


	winAttribs.border_pixel			= 0;
	winAttribs.background_pixmap	= 0;
	winAttribs.colormap 			= XCreateColormap(gpDisplay,
										RootWindow(gpDisplay, gpXVisualInfo->screen),
										gpXVisualInfo->visual,
										AllocNone);
	
	gColormap						= winAttribs.colormap;

	winAttribs.background_pixel 	= BlackPixel(gpDisplay, defaultScreen);
	winAttribs.event_mask 			= ExposureMask	|	VisibilityChangeMask	|	ButtonPressMask			|
								  	  KeyPressMask	|	PointerMotionMask		|	StructureNotifyMask;

	styleMask						= CWBorderPixel	|	CWBackPixel	|	CWEventMask	|	CWColormap;

	gWindow 						= XCreateWindow(gpDisplay,
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
		printf("ERROR : Failed to Create Main Window.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay, gWindow, "Somshekhar Karle");

	Atom windowManagerDelete	=	XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);
}

void ToggleFullScreen(void)
{
	//variable declarations
	Atom wm_state;
	Atom fullscreen;
	XEvent xev	= {0};
	

	//code
	memset(&xev, 0, sizeof(xev));

	wm_state					= XInternAtom(gpDisplay, "_NET_WM_STATE", False);

	xev.type					= ClientMessage;
	xev.xclient.window 			= gWindow;
	xev.xclient.message_type 	= wm_state;
	xev.xclient.format 			= 32;
	xev.xclient.data.l[0] 		= bFullScreen ? 0 : 1;

	fullscreen 					= XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] 		= fullscreen;

	XSendEvent(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), False, StructureNotifyMask, &xev);

}

void Initialize()
{
	void Resize(int, int);
	
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//////////////////////////////////////////////////////////////////
	// Initialization of light
	/////////////////////////////////////////////////////////////////

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecularZero);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecularOne);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecularTwo);
	glEnable(GL_LIGHT2);

	//////////////////////////////////////////////////////////////////
	// Material Initialization
	//////////////////////////////////////////////////////////////////

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	Resize(giWindowWidth, giWindowHeight);
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void Draw(void)
{
		//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
		gluLookAt
		(
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		);

		glPushMatrix();
			glRotatef(lightAngle0, 1.0f, 0.0f, 0.0f);
			lightPositionZero[1] = lightAngle0;
			glLightfv(GL_LIGHT0, GL_POSITION, lightPositionZero);
		glPopMatrix();

		glPushMatrix();
			glRotatef(lightAngle1, 0.0f, 1.0f, 0.0f);
			lightPositionOne[0] = lightAngle1;
			glLightfv(GL_LIGHT1, GL_POSITION, lightPositionOne);
		glPopMatrix();

		glPushMatrix();
			glRotatef(lightAngle2, 0.0f, 0.0f, 1.0f);
			lightPositionTwo[0] = lightAngle2;
			glLightfv(GL_LIGHT2, GL_POSITION, lightPositionTwo);
		glPopMatrix();

	glTranslatef(0.0f, 0.0f, -1.0f);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f, 30, 30);	// Normals are calculated by gluSpere it self, we dont need to write Normal function explicitly.
	
	glPopMatrix();

	lightAngle0 = lightAngle0 + 0.2f;
	if (lightAngle0 >= 360.0f)
	{
		lightAngle0 = 0.0f;
	}

	lightAngle1 = lightAngle1 + 0.2f;
	if (lightAngle1 >= 360.0f)
	{
		lightAngle1 = 0.0f;
	}

	lightAngle2 = lightAngle2 + 0.2f;
	if (lightAngle2 >= 360.0f)
	{
		lightAngle2 = 0.0f;
	}

	glXSwapBuffers(gpDisplay, gWindow);
}

void uninitialize(void)
{
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
}