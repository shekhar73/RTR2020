#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

//namespaces
using namespace std;

//global variable declarations
bool bFullScreen			= false;
Display *gpDisplay			= NULL;
XVisualInfo *gpXVisualInfo	= NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;

int giWindowWidth			= 800;
int giWindowHeight			= 600;

int sholder = 0;
int elbow = 0;

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
	char keys[26];

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

					case 'S':
						sholder = (sholder + 3) % 360;
						break;

					case 's':
						sholder = (sholder - 3) % 360;
						break;

					case 'E':
						elbow = (elbow + 3) % 360;
						break;

					case 'e':
						elbow = (elbow - 3) % 360;
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
	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-1.0f, 0.0f, -3.0f);

	glBegin(GL_QUADS);

		glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

		glVertex3f(0.05f, 0.2f, 0.0f);		//top right corner
		glVertex3f(-0.05f, 0.2f, 0.0f);		//top left corner
		glVertex3f(-0.05f, 0.2f, 0.0f);		//top left corner

		glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR
		glVertex3f(-0.05f, -0.2f, 0.0f);	//bottom left corner	
		glVertex3f(-0.05f, -0.2f, 0.0f);	//bottom left corner
		glVertex3f(0.05f, -0.2f, 0.0f);		//bottom right corner
		glVertex3f(0.05f, -0.2f, 0.0f);		//bottom right corner
	
		glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
		glVertex3f(0.05f, 0.2f, 0.0f);		//top right corner


	glEnd();

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of I
	////////////////////////////////////////////////////////////////////////////////////////////

	

	////////////////////////////////////////////////////////////////////////////////////////////
	// N
	////////////////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.6f, 0.0f, -3.0f);

	glColor3f(0.05, 0.5f, 0.0f);

	glBegin(GL_QUADS);

		//Right Vertical Line
		glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
		glVertex3f(0.2f, 0.2f, 0.0f);		//top right corner
		glVertex3f(0.1f, 0.2f, 0.0f);		//top left corner
		glVertex3f(0.1f, 0.2f, 0.0f);		//top left corner

		glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR
		glVertex3f(0.1f, -0.2f, 0.0f);		//bottom left corner
		glVertex3f(0.1f, -0.2f, 0.0f);		//bottom left corner
		glVertex3f(0.2f, -0.2f, 0.0f);		//bottom right corner
		glVertex3f(0.2f, -0.2f, 0.0f);		//bottom right corner

		glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
		glVertex3f(0.2f, 0.2f, 0.0f);		//top right corner

		//Left Vertical Line
		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(-0.1f, 0.2f, 0.0f);		
		glVertex3f(-0.2f, 0.2f, 0.0f);	
		glVertex3f(-0.2f, 0.2f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(-0.2f, -0.2f, 0.0f); 
		glVertex3f(-0.2f, -0.2f, 0.0f);
		glVertex3f(-0.1f, -0.2f, 0.0f);	
		glVertex3f(-0.1f, -0.2f, 0.0f);	

		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(-0.1f, 0.2f, 0.0f);


		//Cross line from top lert corner to bottom right corner
		glVertex3f(-0.1f, 0.2f, 0.0f);
		glVertex3f(-0.2f, 0.2f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(0.1f, -0.2f, 0.0f);
		glVertex3f(0.2f, -0.2f, 0.0f);
	
	glEnd();

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of N
	////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////
	// D
	////////////////////////////////////////////////////////////////////////////////////////////

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);

	glColor3f(0.05, 0.5f, 0.0f);

	glBegin(GL_QUADS);

		//TOP HORIZONTAL LINE
		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(0.2f, 0.2f, 0.0f);
		glVertex3f(-0.27f, 0.2f, 0.0f);

		glVertex3f(-0.27f, 0.2f, 0.0f);
		glVertex3f(-0.27f, 0.1f, 0.0f);

		glVertex3f(-0.27f, 0.1f, 0.0f);
		glVertex3f(0.2f, 0.1f, 0.0f); 

		glVertex3f(0.2f, 0.1f, 0.0f);
		glVertex3f(0.2f, 0.2f, 0.0f);


		//LEFT VERTICAL LINE
		glVertex3f(-0.1f, 0.2f, 0.0f);
		glVertex3f(-0.2f, 0.2f, 0.0f);
		glVertex3f(-0.2f, 0.2f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(-0.2f, -0.2f, 0.0f);
		glVertex3f(-0.2f, -0.2f, 0.0f);
		glVertex3f(-0.1f, -0.2f, 0.0f);
		glVertex3f(-0.1f, -0.2f, 0.0f);

		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(-0.1f, 0.2f, 0.0f);


		//BOTTOM HORIZONTAL LINE
		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(-0.27f, -0.2f, 0.0f);
		glVertex3f(0.2f, -0.2f, 0.0f);

		glVertex3f(0.2f, -0.2f, 0.0f);
		glVertex3f(0.2f, -0.1f, 0.0f);

		glVertex3f(0.2f, -0.1f, 0.0f);
		glVertex3f(-0.27f, -0.1f, 0.0f);

		glVertex3f(-0.27f, -0.1f, 0.0f);
		glVertex3f(-0.27f, -0.2f, 0.0f);


		//RIGHT VERTICAL LINE
		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(0.1f, -0.2f, 0.0f);
		glVertex3f(0.2f, -0.2f, 0.0f);

		glVertex3f(0.2f, -0.2f, 0.0f);

		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(0.2f, 0.2f, 0.0f);

		glVertex3f(0.2f, 0.2f, 0.0f);
		glVertex3f(0.1f, 0.2f, 0.0f);

		glVertex3f(0.1f, 0.2f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(0.1f, -0.2f, 0.0f);

	glEnd();
	////////////////////////////////////////////////////////////////////////////////////////////
	// End of D
	////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////
	// I
	////////////////////////////////////////////////////////////////////////////////////////////

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.4f, 0.0f, -3.0f);

	glColor3f(0.05, 0.5f, 0.0f);

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR

	glVertex3f(0.05f, 0.2f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.2f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.2f, 0.0f);		//top left corner

	glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
	glVertex3f(-0.05f, -0.2f, 0.0f);	//bottom left corner

	glVertex3f(-0.05f, -0.2f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.2f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.2f, 0.0f);		//bottom right corner

	glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
	glVertex3f(0.05f, 0.2f, 0.0f);		//top right corner


	glEnd();

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of I
	////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////
	// A
	////////////////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.7f, 0.0f, -3.0f);

	glColor3f(0.05, 0.5f, 0.0f);

	glBegin(GL_QUADS);

		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR

		//Cross from apex to left bottom
		glVertex3f(0.05f, 0.2f, 0.0f);   
		glVertex3f(-0.05f, 0.2f, 0.0f);

		glVertex3f(-0.05f, 0.2f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(-0.2f, -0.2f, 0.0f);

		glVertex3f(-0.2f, -0.2f, 0.0f);
		glVertex3f(-0.1f, -0.2f, 0.0f);

		glVertex3f(-0.1f, -0.2f, 0.0f);
		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(0.05f, 0.2f, 0.0f);


		//Cross from apex to right bottom
		glVertex3f(-0.05f, 0.2f, 0.0f);
		glVertex3f(0.05f, 0.2f, 0.0f);

		glVertex3f(0.05f, 0.2f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN COLOR
		glVertex3f(0.1f, -0.2f, 0.0f);

		glVertex3f(0.1f, -0.2f, 0.0f);
		glVertex3f(0.2f, -0.2f, 0.0f);

		glVertex3f(0.2f, -0.2f, 0.0f);

		glColor3f(1.0f, 0.5f, 0.0f);	//ORANGE COLOR
		glVertex3f(0.05f, 0.2f, 0.0f);


		//MIDDLE LINE ORANGE
		glColor3f(1.0f, 0.5f, 0.0f);
		glVertex3f(-0.030f, 0.0f, 0.0f);
		glVertex3f(-0.040f, -0.033f, 0.0f);

		glVertex3f(-0.040f, -0.033f, 0.0f);
		glVertex3f(0.040f, -0.033f, 0.0f);

		glVertex3f(0.040f, -0.033f, 0.0f);
		glVertex3f(0.030f, 0.0f, 0.0f);

		glVertex3f(0.030f, 0.0f, 0.0f);
		glVertex3f(-0.030f, 0.0f, 0.0f);



		//MIDDLE LINE WHITE
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.037f, -0.033f, 0.0f);
		glVertex3f(-0.042f, -0.046f, 0.0f);

		glVertex3f(-0.042f, -0.046f, 0.0f);
		glVertex3f(0.041f, -0.046f, 0.0f);

		glVertex3f(0.041f, -0.046f, 0.0f);
		glVertex3f(0.041f, -0.033f, 0.0f);

		glVertex3f(0.037f, -0.033f, 0.0f);
		glVertex3f(-0.037f, -0.033f, 0.0f);


		//MIDDLE LINE GREEN
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.044f, -0.046f, 0.0f);
		glVertex3f(-0.052f, -0.063f, 0.0f);

		glVertex3f(-0.052f, -0.063f, 0.0f);
		glVertex3f(0.052f, -0.063f, 0.0f);

		glVertex3f(0.052f, -0.063f, 0.0f);
		glVertex3f(0.044f, -0.046f, 0.0f);

		glVertex3f(0.044f, -0.046f, 0.0f);
		glVertex3f(-0.044f, -0.046f, 0.0f);


	glEnd();


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