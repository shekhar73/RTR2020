#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#define PI 3.14f

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
	

	GLfloat i = 1.0f, j = 1.0f;
	static int k = 0;
	GLfloat angle = 0.0f, GLj = 1.0f;
	const int circlePoints = 333;
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	static GLfloat z = -9.0f;

	static GLfloat t_angle = 0.0f;

	void IAFPlane(void);


	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0f, 0.0f, z);

	//////////////////////////////////////////////////////////////////////////////
	//Circle
	//////////////////////////////////////////////////////////////////////////////


	glRotatef(t_angle, x, y, z);

	glBegin(GL_LINES);

	k = 0;
	for (GLj = 1.0f; GLj <= 30.0f; GLj += 0.05f)
	{
		k++;
		if (k == 1)
		{
			glColor3f(1.0f, 0.5f, 0.0f);
		}
		if (k == 2)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		if (k == 3)
		{
			glColor3f(0.0, 1.0, 0.0); //Green
			k = 0;
		}

		for (int i = 0; i < circlePoints; i++)
		{
			angle = 2 * PI * i / circlePoints;
			glVertex3f(cos(angle) / GLj, sin(angle) / GLj, 0.0f);
		}

	}


	glEnd();
	z = z + 0.01f;
	if (z >= 9.0f)
	{
		z = -9.0f;
	}
	t_angle += 0.05f;
	if (t_angle >= 360.0f)
	{
		t_angle = 0.0f;
	}

	static GLfloat AxisChange = 0.0018f;
	void callIAF();
	void flag();



	////////////////////////////////////////////////////////////////////////////////////////////
	// I
	////////////////////////////////////////////////////////////////////////////////////////////

	static GLfloat xAxisForI = -2.5f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (xAxisForI <= -1.1f)
	{
		glTranslatef(xAxisForI, 0.0f, -3.0f);
	}
	else
	{
		glTranslatef(-1.1f, 0.0f, -3.0f);
	}


	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner	

	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner




	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner


	glEnd();


	if (xAxisForI <= -1.1f)
	{
		xAxisForI += AxisChange;

	}
	else
	{
		xAxisForI = -1.1f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of I
	////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////
	// N
	////////////////////////////////////////////////////////////////////////////////////////////

	static GLfloat yAxisForN = 1.5f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (yAxisForN >= 0.0f)
	{
		glTranslatef(-0.6f, yAxisForN, -3.0f);
	}
	else
	{
		glTranslatef(-0.6f, 0.0f, -3.0f);
	}



	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(0.3f, 0.3f, 0.0f);
	glVertex3f(0.2f, 0.3f, 0.0f);

	glVertex3f(0.2f, 0.3f, 0.0f);
	glVertex3f(0.2f, 0.1f, 0.0f);

	glVertex3f(0.2f, 0.1f, 0.0f);
	glVertex3f(0.3f, 0.1f, 0.0f);

	glVertex3f(0.3f, 0.1f, 0.0f);
	glVertex3f(0.3f, 0.3f, 0.0f);



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(0.3f, 0.1f, 0.0f);
	glVertex3f(0.2f, 0.1f, 0.0f);

	glVertex3f(0.2f, 0.1f, 0.0f);
	glVertex3f(0.2f, -0.1f, 0.0f);

	glVertex3f(0.2f, -0.1f, 0.0f);
	glVertex3f(0.3f, -0.1f, 0.0f);

	glVertex3f(0.3f, -0.1f, 0.0f);
	glVertex3f(0.3f, 0.1f, 0.0f);


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.3f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.1f, 0.0f);

	glVertex3f(0.2f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.3f, 0.0f);

	glVertex3f(0.2f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.3f, 0.0f);

	glVertex3f(0.3f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.1f, 0.0f);





	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);


	glVertex3f(-0.3f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);

	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.2f, 0.1f, 0.0f);

	glVertex3f(-0.2f, 0.1f, 0.0f);
	glVertex3f(-0.2f, 0.3f, 0.0f);



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);

	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(-0.2f, -0.1f, 0.0f);

	glVertex3f(-0.2f, -0.1f, 0.0f);
	glVertex3f(-0.2f, 0.1f, 0.0f);

	glVertex3f(-0.2f, 0.1f, 0.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(-0.3f, -0.3f, 0.0f);

	glVertex3f(-0.3f, -0.3f, 0.0f);
	glVertex3f(-0.2f, -0.3f, 0.0f);

	glVertex3f(-0.2f, -0.3f, 0.0f);
	glVertex3f(-0.2f, -0.1f, 0.0f);

	glVertex3f(-0.2f, -0.1f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);



	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);

	glVertex3f(-0.3f, 0.3f, 0.0f);
	glVertex3f(-0.13f, 0.1f, 0.0f);

	glVertex3f(-0.13f, 0.1f, 0.0f);
	glVertex3f(-0.03f, 0.1f, 0.0f);

	glVertex3f(-0.03f, 0.1f, 0.0f);
	glVertex3f(-0.2f, 0.3f, 0.0f);


	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.13f, 0.1f, 0.0f);
	glVertex3f(0.03f, -0.1f, 0.0f);

	glVertex3f(0.03f, -0.1f, 0.0f);
	glVertex3f(0.13f, -0.1f, 0.0f);

	glVertex3f(0.13f, -0.1f, 0.0f);
	glVertex3f(-0.03f, 0.1f, 0.0f);

	glVertex3f(-0.03f, 0.1f, 0.0f);
	glVertex3f(-0.13f, 0.1f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR
	glVertex3f(0.03f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.3f, 0.0f);

	glVertex3f(0.2f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.3f, 0.0f);

	glVertex3f(0.3f, -0.3f, 0.0f);
	glVertex3f(0.13f, -0.1f, 0.0f);

	glVertex3f(0.13f, -0.1f, 0.0f);
	glVertex3f(0.03f, -0.1f, 0.0f);

	glEnd();

	if (yAxisForN >= 0.0f)
	{
		yAxisForN -= AxisChange;

	}
	else
	{
		xAxisForI = 0.0f;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of N
	////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// D
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static GLfloat zAxisForD = -50.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (zAxisForD <= -3.0f)
	{
		glTranslatef(0.05f, 0.0f, zAxisForD);
	}
	else
	{
		glTranslatef(0.05f, 0.0f, -3.0f);
	}




	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(-0.1f, 0.3f, 0.0f);	//1
	glVertex3f(-0.2f, 0.3f, 0.0f);	//2

	glVertex3f(-0.2f, 0.3f, 0.0f);	//2
	glVertex3f(-0.2f, 0.1f, 0.0f);	//3

	glVertex3f(-0.2f, 0.1f, 0.0f);	//3
	glVertex3f(-0.1f, 0.1f, 0.0f);	//4

	glVertex3f(-0.1f, 0.1f, 0.0f);	//4
	glVertex3f(-0.1f, 0.3f, 0.0f);	//1


	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.1f, 0.1f, 0.0f);	//4
	glVertex3f(-0.2f, 0.1f, 0.0f);	//3

	glVertex3f(-0.2f, 0.1f, 0.0f);	//3
	glVertex3f(-0.2f, -0.1f, 0.0f);	//6

	glVertex3f(-0.2f, -0.1f, 0.0f);	//6
	glVertex3f(-0.1f, -0.1f, 0.0f);	//5

	glVertex3f(-0.1f, -0.1f, 0.0f);	//5
	glVertex3f(-0.1f, 0.1f, 0.0f);	//4


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(-0.1f, -0.1f, 0.0f);	//5
	glVertex3f(-0.2f, -0.1f, 0.0f);	//6

	glVertex3f(-0.2f, -0.1f, 0.0f);	//6
	glVertex3f(-0.2f, -0.3f, 0.0f);	//7

	glVertex3f(-0.2f, -0.3f, 0.0f);	//7
	glVertex3f(-0.1f, -0.3f, 0.0f);	//8

	glVertex3f(-0.1f, -0.3f, 0.0f);	//8
	glVertex3f(-0.1f, -0.1f, 0.0f);	//5


	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(-0.1f, 0.2f, 0.0);	//9
	glVertex3f(-0.1f, 0.3f, 0.0f);	//1

	glVertex3f(-0.1f, 0.3f, 0.0f);	//1
	glVertex3f(0.1f, 0.3f, 0.0);	//12

	glVertex3f(0.1f, 0.3f, 0.0);	//12
	glVertex3f(0.2f, 0.2f, 0.0);	//11

	glVertex3f(0.2f, 0.2f, 0.0);	//11
	glVertex3f(-0.1f, 0.2f, 0.0);	//9



	glVertex3f(0.05f, 0.2f, 0.0);	//10
	glVertex3f(0.2f, 0.2f, 0.0);	//11


	glVertex3f(0.2f, 0.2f, 0.0);	//11
	glVertex3f(0.2f, 0.15f, 0.0);	//24


	glVertex3f(0.2f, 0.15f, 0.0);	//24
	glVertex3f(0.1f, 0.15f, 0.0);	//23

	glVertex3f(0.1f, 0.15f, 0.0);	//23
	glVertex3f(0.05f, 0.2f, 0.0);	//10



	glVertex3f(0.2f, 0.15f, 0.0);	//24
	glVertex3f(0.1f, 0.15f, 0.0);	//23

	glVertex3f(0.1f, 0.15f, 0.0);	//23
	glVertex3f(0.1f, 0.1f, 0.0);	//14

	glVertex3f(0.1f, 0.1f, 0.0);	//14
	glVertex3f(0.2f, 0.1f, 0.0);	//13

	glVertex3f(0.2f, 0.1f, 0.0);	//13
	glVertex3f(0.2f, 0.15f, 0.0);	//24


	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR
	glVertex3f(0.2f, 0.1f, 0.0);	//13
	glVertex3f(0.1f, 0.1f, 0.0);	//14

	glVertex3f(0.1f, 0.1f, 0.0);	//14
	glVertex3f(0.1f, -0.1f, 0.0f);	//15

	glVertex3f(0.1f, -0.1f, 0.0f);	//15
	glVertex3f(0.2f, -0.1f, 0.0f);	//16

	glVertex3f(0.2f, -0.1f, 0.0f);	//16
	glVertex3f(0.2f, 0.1f, 0.0);	//13



	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.1f, -0.1f, 0.0f);	//15
	glVertex3f(0.2f, -0.1f, 0.0f);	//16

	glVertex3f(0.2f, -0.1f, 0.0f);	//16
	glVertex3f(0.2f, -0.15f, 0.0f);		//21

	glVertex3f(0.2f, -0.15f, 0.0f);		//21
	glVertex3f(0.1f, -0.15f, 0.0f);		//22

	glVertex3f(0.1f, -0.15f, 0.0f);		//22
	glVertex3f(0.1f, -0.1f, 0.0f);	//15



	glVertex3f(0.2f, -0.15f, 0.0f);		//21
	glVertex3f(0.1f, -0.15f, 0.0f);		//22

	glVertex3f(0.1f, -0.15f, 0.0f);		//22
	glVertex3f(0.05f, -0.2f, 0.0f);	//18

	glVertex3f(0.05f, -0.2f, 0.0f);	//18
	glVertex3f(0.2f, -0.2f, 0.0f);	//19

	glVertex3f(0.2f, -0.2f, 0.0f);	//19
	glVertex3f(0.2f, -0.15f, 0.0f);		//21



	glVertex3f(0.2f, -0.2f, 0.0f);		//19
	glVertex3f(-0.1f, -0.2f, 0.0f);		//17

	glVertex3f(-0.1f, -0.2f, 0.0f);		//17
	glVertex3f(-0.1f, -0.3f, 0.0f);		//8

	glVertex3f(-0.1f, -0.3f, 0.0f);		//8
	glVertex3f(0.1f, -0.3f, 0.0f);		//20

	glVertex3f(0.1f, -0.3f, 0.0f);		//20
	glVertex3f(0.2f, -0.2f, 0.0f);		//19


	glEnd();

	if (zAxisForD <= -3.0f)
	{
		zAxisForD += 0.065f;

	}
	else
	{
		zAxisForD = -3.0f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of D
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////
	// I
	////////////////////////////////////////////////////////////////////////////////////////////

	
	static GLfloat NegYAxisForI = -1.5f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	if (NegYAxisForI <= 0.0f)
	{
		glTranslatef(0.45f, NegYAxisForI, -3.0f);
	}
	else
	{
		glTranslatef(0.45f, 0.0f, -3.0f);
	}



	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.3f, 0.0f);		//top left corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner	

	glVertex3f(-0.05f, 0.1f, 0.0f);		//bottom left corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, 0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.3f, 0.0f);		//top right corner



	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, 0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.1f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.1f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, 0.1f, 0.0f);		//top right corner




	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner
	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner

	glVertex3f(-0.05f, -0.1f, 0.0f);		//top left corner
	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner	

	glVertex3f(-0.05f, -0.3f, 0.0f);	//bottom left corner
	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner

	glVertex3f(0.05f, -0.3f, 0.0f);		//bottom right corner
	glVertex3f(0.05f, -0.1f, 0.0f);		//top right corner


	glEnd();


	if (NegYAxisForI <= 0.0f)
	{
		NegYAxisForI += AxisChange;

	}
	else
	{
		NegYAxisForI = 0.0f;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// End of I
	////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// A
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	static GLfloat NegXAxisForA = 2.5f;
	static GLfloat axis = -10.0f;



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (NegXAxisForA >= 0.9f)
	{
		glTranslatef(NegXAxisForA, 0.0f, -3.0f);
	}
	else
	{
		glTranslatef(0.9f, 0.0f, -3.0f);
	}

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR

	glVertex3f(0.06f, 0.3f, 0.0f);	//1
	glVertex3f(-0.06f, 0.3f, 0.0f);	//2

	glVertex3f(-0.06f, 0.3f, 0.0f);	//2
	glVertex3f(-0.13f, 0.1f, 0.0f);	//3

	glVertex3f(-0.13f, 0.1f, 0.0f);	//3
	glVertex3f(-0.03f, 0.1f, 0.0f);	//4

	glVertex3f(-0.03f, 0.1f, 0.0f);	//4
	glVertex3f(0.06f, 0.3f, 0.0f);	//1


	glVertex3f(0.06f, 0.3f, 0.0f);	//1
	glVertex3f(-0.06f, 0.3f, 0.0f);	//2

	glVertex3f(-0.06f, 0.3f, 0.0f);	//2
	glVertex3f(0.03f, 0.1f, 0.0f);	//6

	glVertex3f(0.03f, 0.1f, 0.0f);	//6
	glVertex3f(0.13f, 0.1f, 0.0f);	//7

	glVertex3f(0.13f, 0.1f, 0.0f);	//7
	glVertex3f(0.06f, 0.3f, 0.0f);	//1

	glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

	glVertex3f(-0.13f, 0.1f, 0.0f);	//3
	glVertex3f(-0.19f, -0.1f, 0.0f); //8

	glVertex3f(-0.19f, -0.1f, 0.0f); //8
	glVertex3f(-0.09f, -0.1f, 0.0f);  //9

	glVertex3f(-0.09f, -0.1f, 0.0f);  //9
	glVertex3f(-0.03f, 0.1f, 0.0f);	//4

	glVertex3f(-0.03f, 0.1f, 0.0f);	//4
	glVertex3f(-0.13f, 0.1f, 0.0f);	//3


	glVertex3f(0.03f, 0.1f, 0.0f);	//6
	glVertex3f(0.09f, -0.1f, 0.0f);	//10

	glVertex3f(0.09f, -0.1f, 0.0f);	//10
	glVertex3f(0.19f, -0.1f, 0.0f);	//11

	glVertex3f(0.19f, -0.1f, 0.0f);	//11
	glVertex3f(0.13f, 0.1f, 0.0f);	//7

	glVertex3f(0.13f, 0.1f, 0.0f);	//7
	glVertex3f(0.03f, 0.1f, 0.0f);	//6


	glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR

	glVertex3f(-0.19f, -0.1f, 0.0f); //8
	glVertex3f(-0.25f, -0.3f, 0.0f);	//12

	glVertex3f(-0.25f, -0.3f, 0.0f);	//12
	glVertex3f(-0.15f, -0.3f, 0.0f);	//13

	glVertex3f(-0.15f, -0.3f, 0.0f);	//13
	glVertex3f(-0.09f, -0.1f, 0.0f);  //9

	glVertex3f(-0.09f, -0.1f, 0.0f);  //9
	glVertex3f(-0.19f, -0.1f, 0.0f); //8


	glVertex3f(0.09f, -0.1f, 0.0f);	//10
	glVertex3f(0.15f, -0.3f, 0.0f);	//14

	glVertex3f(0.15f, -0.3f, 0.0f);	//14
	glVertex3f(0.25f, -0.3f, 0.0f);	//15

	glVertex3f(0.25f, -0.3f, 0.0f);	//15
	glVertex3f(0.19f, -0.1f, 0.0f);	//11

	glVertex3f(0.19f, -0.1f, 0.0f);	//11
	glVertex3f(0.09f, -0.1f, 0.0f);	//10



	if (axis >= 5.8f)
	{
		//////////////////////////////////////////////////////
		glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
		glVertex3f(0.05f, 0.030f, 0.0f);	//a
		glVertex3f(-0.05f, 0.030f, 0.0f);	//b

		glVertex3f(-0.05f, 0.030f, 0.0f);	//b
		glVertex3f(-0.057f, 0.010f, 0.0f);	//c

		glVertex3f(-0.057f, 0.010f, 0.0f);	//c
		glVertex3f(0.057f, 0.010f, 0.0f);	//d

		glVertex3f(0.057f, 0.010f, 0.0f);	//d
		glVertex3f(0.05f, 0.030f, 0.0f);	//a


		glColor3f(1.0f, 1.0f, 1.0f);		//WHITE COLOR

		glVertex3f(0.057f, 0.010f, 0.0f);	//d
		glVertex3f(-0.057f, 0.010f, 0.0f);	//c

		glVertex3f(-0.057f, 0.010f, 0.0f);	//c
		glVertex3f(-0.063f, -0.010f, 0.0f);	//f

		glVertex3f(-0.063f, -0.010f, 0.0f);	//f
		glVertex3f(0.063f, -0.010f, 0.0f);	//e

		glVertex3f(0.063f, -0.010f, 0.0f);	//e
		glVertex3f(0.056f, 0.010f, 0.0f);	//d


		glColor3f(0.0f, 1.0f, 0.0f);		//GREEN COLOR
		glVertex3f(0.063f, -0.010f, 0.0f);	//e
		glVertex3f(-0.063f, -0.010f, 0.0f);	//f

		glVertex3f(-0.063f, -0.010f, 0.0f);	//f
		glVertex3f(-0.069f, -0.030f, 0.0f);	//g

		glVertex3f(-0.069f, -0.030f, 0.0f);	//g
		glVertex3f(0.069f, -0.030f, 0.0f);	//h

		glVertex3f(0.069f, -0.030f, 0.0f);	//h
		glVertex3f(0.063f, -0.010f, 0.0f);	//e
	}


	glEnd();

	if (NegXAxisForA >= 0.9f)
	{
		NegXAxisForA -= AxisChange;

	}
	else
	{
		NegXAxisForA = 0.9f;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End Of A
	////////////////////////////////////////////////////////////////////////////////////////////////////////


	if (NegYAxisForI >= -0.2f)
	{

		/////////Stright PLANE///////////////////Stright PLANE//////////////////Stright PLANE///////////////Stright PLANE//////////////////////////Stright PLANE/////////////
		static GLfloat planeAxisChange = 0.011f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (axis <= 15.0f)
		{
			glTranslatef(axis, 0.0f, -6.0f);
		}

		glBegin(GL_QUADS);

		flag();
		IAFPlane();

		glEnd();

		axis += planeAxisChange;
		if (axis >= 12.0f)
		{
			axis = -10.0f;
		}

		/////////Top PLANE///////////////////Top PLANE//////////////////Top PLANE///////////////Top PLANE//////////////////////////Top PLANE/////////////

		static GLfloat topXAxis = -10.0f;
		static GLfloat topYAxis = 5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (topXAxis <= 12.0f)
		{
			glTranslatef(topXAxis, topYAxis, -6.0f);
		}

		glBegin(GL_QUADS);

		flag();
		IAFPlane();

		glEnd();

		topXAxis += planeAxisChange;
		if (topXAxis >= 12.0f)
		{
			topXAxis = -10.0f;
		}

		if (topXAxis >= 6.5f)
		{
			topYAxis += planeAxisChange;
		}

		else if (topYAxis <= 0.0f)
		{
			topYAxis = 0.0f;
		}
		else
		{
			topYAxis -= planeAxisChange;
		}

		/////////BOTTOM PLANE///////////////////BOTTOM PLANE//////////////////BOTTOM PLANE///////////////BOTTOM PLANE//////////////////////////BOTTOM PLANE/////////////

		static GLfloat bottomXAxis = -10.0f;
		static GLfloat bottomYAxis = -5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (bottomXAxis <= 12.0f)
		{
			glTranslatef(bottomXAxis, bottomYAxis, -6.0f);
		}

		glBegin(GL_QUADS);
		flag();
		IAFPlane();
		glEnd();

		bottomXAxis += planeAxisChange;
		if (bottomXAxis >= 12.0f)
		{
			bottomXAxis = -10.0f;
		}

		if (bottomXAxis >= 6.5f)
		{
			bottomYAxis -= planeAxisChange;
		}

		else if (bottomYAxis >= 0.0f)
		{
			bottomYAxis = 0.0f;
		}
		else
		{
			bottomYAxis += planeAxisChange;
		}



		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IAF word on Top Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static GLfloat topXAxisIAF = -10.0f;
		static GLfloat topYAxisIAF = 5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (topXAxisIAF <= 12.0f)
		{
			glTranslatef(topXAxisIAF, topYAxisIAF, -6.0f);
		}

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);		//WHITE COLOR

		callIAF();

		glEnd();
		topXAxisIAF += planeAxisChange;
		if (topXAxisIAF >= 12.0f)
		{
			topXAxisIAF = -10.0f;
		}

		if (topXAxisIAF >= 6.5f)
		{
			topYAxisIAF += planeAxisChange;
		}

		else if (topYAxisIAF <= 0.0f)
		{
			topYAxisIAF = 0.0f;
		}
		else
		{
			topYAxisIAF -= planeAxisChange;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// End Of Top IAF on Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IAF word on straight Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static GLfloat axis_i = -10.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (axis_i <= 15.0f)
		{
			glTranslatef(axis_i, 0.0f, -6.0f);
		}

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);		//WHITE COLOR

		callIAF();

		glEnd();
		axis_i += planeAxisChange;
		if (axis_i >= 12.0f)
		{
			axis_i = -10.0f;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Straight	End of IAF on Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// IAF word on Bottom Plane
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		static GLfloat bottomXAxisI = -10.0f;
		static GLfloat bottomYAxisI = -5.0f;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		if (bottomXAxisI <= 15.0f)
		{
			glTranslatef(bottomXAxisI, bottomYAxisI, -6.0f);
		}

		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);		//WHITE COLOR

		callIAF();

		glEnd();

		bottomXAxisI += planeAxisChange;
		if (bottomXAxisI >= 12.0f)
		{
			bottomXAxisI = -10.0f;
		}

		if (bottomXAxisI >= 6.5f)
		{
			bottomYAxisI -= planeAxisChange;
		}
		else if (bottomYAxisI >= 0.0f)
		{
			bottomYAxisI = 0.0f;
		}
		else
		{
			bottomYAxisI += planeAxisChange;
		}

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of Bottom of IAF on Plane
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glEnd();

	glXSwapBuffers(gpDisplay, gWindow);
}



void IAFPlane()
{
	static long int iNum = 5000000;


	glColor3f(0.3f, 0.3f, 0.3f);	//Gray Color
	glVertex3f(1.0f, 0.0f, 0.0f);	//1
	glVertex3f(0.7f, 0.1f, 0.0f);	//2


	glVertex3f(0.7f, 0.1f, 0.0f);	//2
	glVertex3f(0.7f, 0.0f, 0.0f);	//38

	glVertex3f(0.7f, 0.0f, 0.0f);	//38
	glVertex3f(0.7f, -0.1f, 0.0f);	//34

	glVertex3f(0.7f, -0.1f, 0.0f);	//34
	glVertex3f(1.0f, 0.0f, 0.0f);	//1

	glColor3f(0.6f, 0.6f, 0.6f);	//Light gray color
	glVertex3f(0.7f, 0.1f, 0.0f);	//2
	glVertex3f(0.4f, 0.15f, 0.0f);	//3

	glVertex3f(0.4f, 0.15f, 0.0f);	//3
	glVertex3f(0.4f, -0.15f, 0.0f);	//33

	glVertex3f(0.4f, -0.15f, 0.0f);	//33
	glVertex3f(0.7f, -0.1f, 0.0f);	//34

	glVertex3f(0.7f, -0.1f, 0.0f);	//34
	glVertex3f(0.7f, 0.1f, 0.0f);	//2




	glVertex3f(0.4f, 0.15f, 0.0f);	//3
	glVertex3f(0.2f, 0.3f, 0.0f);	//4

	glVertex3f(0.2f, 0.3f, 0.0f);	//4
	glVertex3f(0.15f, 0.3f, 0.0f);	//5

	glVertex3f(0.15f, 0.3f, 0.0f);	//5
	glVertex3f(0.2f, 0.15f, 0.0f);	//6

	glVertex3f(0.2f, 0.15f, 0.0f);	//6
	glVertex3f(0.4f, 0.15f, 0.0f);	//3


	glVertex3f(0.4f, 0.15f, 0.0f);	//3
	glVertex3f(0.2f, 0.15f, 0.0f);	//6

	glVertex3f(0.2f, 0.15f, 0.0f);	//6
	glVertex3f(0.2f, -0.15f, 0.0f);	//30

	glVertex3f(0.2f, -0.15f, 0.0f);	//30
	glVertex3f(0.4f, -0.15f, 0.0f);	//33

	glVertex3f(0.4f, -0.15f, 0.0f);	//33
	glVertex3f(0.4f, 0.15f, 0.0f);	//3




	glVertex3f(0.4f, -0.15f, 0.0f);	//33
	glVertex3f(0.2f, -0.15f, 0.0f);	//30

	glVertex3f(0.2f, -0.15f, 0.0f);	//30
	glVertex3f(0.15f, -0.3f, 0.0f);	//31

	glVertex3f(0.15f, -0.3f, 0.0f);	//31
	glVertex3f(0.2f, -0.3f, 0.0f);	//32

	glVertex3f(0.2f, -0.3f, 0.0f);	//32
	glVertex3f(0.4f, -0.15f, 0.0f);	//33



	glVertex3f(0.2f, 0.15f, 0.0f);	//6
	glVertex3f(0.15f, 0.17f, 0.0f);	//7

	glVertex3f(0.15f, 0.17f, 0.0f);	//7
	glVertex3f(0.15f, -0.17f, 0.0f);	//29

	glVertex3f(0.15f, -0.17f, 0.0f);	//29
	glVertex3f(0.2f, -0.15f, 0.0f);	//30

	glVertex3f(0.2f, -0.15f, 0.0f);	//30
	glVertex3f(0.2f, 0.15f, 0.0f);	//6



	glVertex3f(0.15f, 0.17f, 0.0f);	//7
	glVertex3f(-0.3f, 0.6f, 0.0f);	//8

	glVertex3f(-0.3f, 0.6f, 0.0f);	//8
	glVertex3f(-0.5f, 0.6f, 0.0f);	//9

	glVertex3f(-0.5f, 0.6f, 0.0f);	//9
	glVertex3f(-0.3f, 0.15f, 0.0f);	//10

	glVertex3f(-0.3f, 0.15f, 0.0f);	//10
	glVertex3f(0.15f, 0.17f, 0.0f);	//7


	glVertex3f(0.15f, 0.17f, 0.0f);	//7
	glVertex3f(-0.3f, 0.15f, 0.0f);	//10
	
	glVertex3f(-0.3f, 0.15f, 0.0f);	//10
	glVertex3f(-0.3f, -0.15f, 0.0f);	//26


	glVertex3f(-0.3f, -0.15f, 0.0f);	//26
	glVertex3f(0.15f, -0.17f, 0.0f);	//29

	glVertex3f(0.15f, -0.17f, 0.0f);	//29
	glVertex3f(0.15f, 0.17f, 0.0f);	//7



	glVertex3f(-0.3f, -0.15f, 0.0f);	//26
	glVertex3f(-0.5f, -0.6f, 0.0f);	//27

	glVertex3f(-0.5f, -0.6f, 0.0f);	//27
	glVertex3f(-0.3f, -0.6f, 0.0f);	//28

	glVertex3f(-0.3f, -0.6f, 0.0f);	//28
	glVertex3f(0.15f, -0.17f, 0.0f);	//29

	glVertex3f(0.15f, -0.17f, 0.0f);	//29
	glVertex3f(-0.3f, -0.15f, 0.0f);	//26


	glVertex3f(-0.35f, 0.15f, 0.0f);	//12
	glVertex3f(-0.7f, 0.4f, 0.0f);	//13

	glVertex3f(-0.7f, 0.4f, 0.0f);	//13
	glVertex3f(-0.8f, 0.35f, 0.0f);	//14

	glVertex3f(-0.8f, 0.35f, 0.0f);	//14
	glVertex3f(-0.75f, 0.15f, 0.0f);	//15

	glVertex3f(-0.75f, 0.15f, 0.0f);	//15
	glVertex3f(-0.35f, 0.15f, 0.0f);	//12



	glColor3f(0.3f, 0.3f, 0.3f);	//Gray Color

	glVertex3f(-0.25f, 0.15f, 0.0f);	//11
	glVertex3f(-0.8f, 0.15f, 0.0f);	//16

	glVertex3f(-0.8f, 0.15f, 0.0f);	//16
	glVertex3f(-0.8f, 0.05f, 0.0f);	//17

	glVertex3f(-0.8f, 0.05f, 0.0f);	//17
	glVertex3f(-0.25f, 0.05f, 0.0f);	//36

	glVertex3f(-0.25f, 0.05f, 0.0f);	//36
	glVertex3f(-0.25f, 0.15f, 0.0f);	//11



	glColor3f(0.6f, 0.6f, 0.6f);	//Light gray color
	glVertex3f(-0.25f, 0.05f, 0.0f);	//36
	glVertex3f(-0.9f, 0.05f, 0.0f);	//18

	glVertex3f(-0.9f, 0.05f, 0.0f);	//18
	glVertex3f(-0.9f, -0.05f, 0.0f);	//19

	glVertex3f(-0.9f, -0.05f, 0.0f);	//19
	glVertex3f(-0.25f, -0.05f, 0.0f);	//37

	glVertex3f(-0.25f, -0.05f, 0.0f);	//37
	glVertex3f(-0.25f, 0.05f, 0.0f);	//36


	glColor3f(0.3f, 0.3f, 0.3f);	//Gray Color
	glVertex3f(-0.25f, -0.05f, 0.0f);	//37
	glVertex3f(-0.8f, -0.05f, 0.0f);	//20

	glVertex3f(-0.8f, -0.05f, 0.0f);	//20
	glVertex3f(-0.8f, -0.15f, 0.0f);	//21

	glVertex3f(-0.8f, -0.15f, 0.0f);	//21
	glVertex3f(-0.25f, -0.15f, 0.0f);	//25

	glVertex3f(-0.25f, -0.15f, 0.0f);	//25
	glVertex3f(-0.25f, -0.05f, 0.0f);	//37


	glColor3f(0.6f, 0.6f, 0.6f);	//Light gray color
	glVertex3f(-0.35f, -0.15f, 0.0f);	//24
	glVertex3f(-0.75f, -0.15f, 0.0f);	//35

	glVertex3f(-0.75f, -0.15f, 0.0f);	//35
	glVertex3f(-0.8f, -0.35f, 0.0f);	//22

	glVertex3f(-0.8f, -0.35f, 0.0f);	//22
	glVertex3f(-0.7f, -0.4f, 0.0f);	//23

	glVertex3f(-0.7f, -0.4f, 0.0f);	//23
	glVertex3f(-0.35f, -0.15f, 0.0f);	//24




	if ((iNum % 2) == 0)
	{
		////////////////////////////////////////////////TOP LIGHT////////////////////////////////////////
		glColor3f(1.0f, 0.0f, 0.0f);	//RED
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8
		glVertex3f(-0.23f, 0.6f, 0.0f);	//50

		glVertex3f(-0.23f, 0.6f, 0.0f);	//50
		glVertex3f(-0.23f, 0.57f, 0.0f);	//51

		glVertex3f(-0.23f, 0.57f, 0.0f);	//51
		glVertex3f(-0.28f, 0.57f, 0.0f);	//52

		glVertex3f(-0.28f, 0.57f, 0.0f);	//52
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8


		glColor3f(0.0f, 0.0f, 1.0f);	//BLUE
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58
		glVertex3f(-0.17f, 0.55f, 0.0f);	//55

		glVertex3f(-0.17f, 0.55f, 0.0f);	//55
		glVertex3f(-0.17f, 0.51f, 0.0f);	//56

		glVertex3f(-0.17f, 0.51f, 0.0f);	//56
		glVertex3f(-0.2f, 0.51f, 0.0f);	//57

		glVertex3f(-0.2f, 0.51f, 0.0f);	//57
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58



		////////////////////////////////////////////////Bottom LIGHT////////////////////////////////////////

		glColor3f(1.0f, 0.0f, 0.0f);	//RED
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71	
		glVertex3f(-0.28f, -0.57f, 0.0f);	//72

		glVertex3f(-0.28f, -0.57f, 0.0f);	//72
		glVertex3f(-0.3f, -0.6f, 0.0f);	//28

		glVertex3f(-0.3f, -0.6f, 0.0f);	//28
		glVertex3f(-0.23f, -0.6f, 0.0f);	//70

		glVertex3f(-0.23f, -0.6f, 0.0f);	//70
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71




		glColor3f(0.0f, 0.0f, 1.0f);	//BLUE
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61
		glVertex3f(-0.17f, -0.51f, 0.0f);	//60

		glVertex3f(-0.17f, -0.51f, 0.0f);	//60
		glVertex3f(-0.17f, -0.55f, 0.0f);	//63

		glVertex3f(-0.17f, -0.55f, 0.0f);	//63
		glVertex3f(-0.25f, -0.55f, 0.0f);	//62

		glVertex3f(-0.25f, -0.55f, 0.0f);	//62
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61
	}
	else
	{
		////////////////////////////////////////////////TOP LIGHT////////////////////////////////////////
		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8
		glVertex3f(-0.23f, 0.6f, 0.0f);	//50

		glVertex3f(-0.23f, 0.6f, 0.0f);	//50
		glVertex3f(-0.23f, 0.57f, 0.0f);	//51

		glVertex3f(-0.23f, 0.57f, 0.0f);	//51
		glVertex3f(-0.28f, 0.57f, 0.0f);	//52

		glVertex3f(-0.28f, 0.57f, 0.0f);	//52
		glVertex3f(-0.3f, 0.6f, 0.0f);	//8


		glColor3f(1.0f, 1.0f, 1.0f);	//WHITE
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58
		glVertex3f(-0.17f, 0.55f, 0.0f);	//55

		glVertex3f(-0.17f, 0.55f, 0.0f);	//55
		glVertex3f(-0.17f, 0.51f, 0.0f);	//56

		glVertex3f(-0.17f, 0.51f, 0.0f);	//56
		glVertex3f(-0.2f, 0.51f, 0.0f);	//57

		glVertex3f(-0.2f, 0.51f, 0.0f);	//57
		glVertex3f(-0.25f, 0.55f, 0.0f);	//58



		////////////////////////////////////////////////Bottom LIGHT////////////////////////////////////////

		glColor3f(0.0f, 1.0f, 0.0f);	//GREEN
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71	
		glVertex3f(-0.28f, -0.57f, 0.0f);	//72

		glVertex3f(-0.28f, -0.57f, 0.0f);	//72
		glVertex3f(-0.3f, -0.6f, 0.0f);	//28

		glVertex3f(-0.3f, -0.6f, 0.0f);	//28
		glVertex3f(-0.23f, -0.6f, 0.0f);	//70

		glVertex3f(-0.23f, -0.6f, 0.0f);	//70
		glVertex3f(-0.23f, -0.57f, 0.0f);	//71




		glColor3f(1.0f, 1.0f, 1.0f);	//WHITE
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61
		glVertex3f(-0.17f, -0.51f, 0.0f);	//60

		glVertex3f(-0.17f, -0.51f, 0.0f);	//60
		glVertex3f(-0.17f, -0.55f, 0.0f);	//63

		glVertex3f(-0.17f, -0.55f, 0.0f);	//63
		glVertex3f(-0.25f, -0.55f, 0.0f);	//62

		glVertex3f(-0.25f, -0.55f, 0.0f);	//62
		glVertex3f(-0.2f, -0.5f, 0.0f);	//61


	}
	iNum -= 1;
}
void flag()
{
	glColor3f(1.0f, 0.5f, 0.0f);		//ORANGE COLOR
	glVertex3f(-0.9f, 0.10f, 0.0f);	//26
	glVertex3f(-0.9f, 0.01f, 0.0f);	//27

	glVertex3f(-0.9f, 0.01f, 0.0f);	//27
	glVertex3f(-2.5f, 0.01f, 0.0f);	//31

	glVertex3f(-2.5f, 0.01f, 0.0f);	//31
	glVertex3f(-2.5f, 0.10f, 0.0f);	//30

	glVertex3f(-2.5f, 0.10f, 0.0f);	//30
	glVertex3f(-0.9f, 0.10f, 0.0f);	//26


	//WHITE
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.9f, 0.01f, 0.0f);	//27
	glVertex3f(-2.5f, 0.01f, 0.0f);	//31

	glVertex3f(-2.5f, 0.01f, 0.0f);	//31
	glVertex3f(-2.5f, -0.07f, 0.0f);	//32

	glVertex3f(-2.5f, -0.07f, 0.0f);	//32
	glVertex3f(-0.9f, -0.07f, 0.0f);	//28

	glVertex3f(-0.9f, -0.07f, 0.0f);	//28
	glVertex3f(-0.9f, 0.01f, 0.0f);	//27

	//GREEN
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.9f, -0.07f, 0.0f);	//28
	glVertex3f(-2.5f, -0.07f, 0.0f);	//32

	glVertex3f(-2.5f, -0.07f, 0.0f);	//32
	glVertex3f(-2.5f, -0.14f, 0.0f);	//33

	glVertex3f(-2.5f, -0.14f, 0.0f);	//33
	glVertex3f(-0.9f, -0.14f, 0.0f);	//29

	glVertex3f(-0.9f, -0.14f, 0.0f);	//29
	glVertex3f(-0.9f, -0.07f, 0.0f);	//28
}
void callIAF()
{
	// I
	glVertex3f(0.0f, 0.1f, 0.0f);
	glVertex3f(0.0f, -0.1f, 0.0f);

	// A
	glVertex3f(0.1f, 0.1f, 0.0f);
	glVertex3f(0.03f, -0.1f, 0.0f);

	glVertex3f(0.1f, 0.1f, 0.0f);
	glVertex3f(0.17f, -0.1f, 0.0f);

	glVertex3f(0.05f, -0.03f, 0.0f);
	glVertex3f(0.15f, -0.03f, 0.0f);

	// F
	glVertex3f(0.21f, 0.1f, 0.0f);
	glVertex3f(0.21f, -0.1f, 0.0f);

	glVertex3f(0.21f, 0.09f, 0.0f);
	glVertex3f(0.31f, 0.09f, 0.0f);

	glVertex3f(0.21f, 0.02f, 0.0f);
	glVertex3f(0.27f, 0.02f, 0.0f);
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