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

#define PI 3.14

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
	
	// Local Functions Declarations
	GLfloat getSidesOfTriangle(GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat getCenter(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat getRadius(GLfloat, GLfloat, GLfloat);
	GLfloat getCenterOfLine(GLfloat, GLfloat);


	// Local Variable Declarations
	GLfloat a, b, c = 0.0f;
	GLfloat inCenterX = 0.0f, inCenterY = 0.0f;
	GLfloat inRadius = 0.0f;
	GLfloat x1_base = 0.0f, y1_base = 0.0f;

	GLfloat angle = 0.0f;
	GLint CirclePoints = 1000;

	GLfloat x1 = 0.0f, y1 = 0.5f;
	GLfloat x2 = -0.5f, y2 = -0.5f;
	GLfloat x3 = 0.5f, y3 = -0.5f;

	static GLfloat x_point = 0.0f, y_point = y1;
	static GLfloat x_point_circle = 1.50f, y_point_circle = 0.75f;
	static GLfloat x_point_triangle = 1.50f, y_point_triangle = 0.75f;
	static GLfloat angle_rotate = 0.0f;

	a = getSidesOfTriangle(x2, y2, x3, y3);
	b = getSidesOfTriangle(x3, y3, x1, y1);
	c = getSidesOfTriangle(x1, y1, x2, y2);

	x1_base = getCenterOfLine(x3, y3);
	y1_base = getCenterOfLine(x2, y2);

	inCenterX = getCenter(a, b, c, x1, x2, x3);
	inCenterY = getCenter(a, b, c, y1, y2, y3);
	inRadius = getRadius(a, b, c);


	//Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	///////////////////////////////////////////////////////////////////////////////////
	// Triangle
	///////////////////////////////////////////////////////////////////////////////////

	if (x_point_triangle < 0.0f)
	{
		glTranslatef(0.0f, 0.0f, -3.0f);
	}
	else
	{
		glTranslatef(-x_point_triangle, -y_point_triangle, -3.0f);
	}
	glRotatef(angle_rotate, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);

	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y2, 0.0f);

	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x3, y3, 0.0f);

	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x1, y1, 0.0f);

	glEnd();

	///////////////////////////////////////////////////////////////////////////////////
	// End of Triangle
	///////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////
	// Circle
	///////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (y_point_circle < 0.0f)
	{
		glTranslatef(inCenterX, inCenterY, -3.0f);
		
	}
	else
	{
		glTranslatef(x_point_circle, -y_point_circle, -3.0f);
	}
	
	glRotatef(angle_rotate, 0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);

	glColor3f(0.0f, 0.0f, 1.0f);

	for (int i = 0; i <= CirclePoints; i++)
	{
		angle = 2 * PI * i / CirclePoints;
		glVertex3f(inRadius * cos(angle), inRadius * sin(angle), 0.0f);
	}

	glEnd();

	///////////////////////////////////////////////////////////////////////////////////
	// End of Circle
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	// Line
	///////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	if (y_point > 0.0f)
	{
		glTranslatef(x_point, y_point, -3.0f);
	}
	else
	{
		glTranslatef(0.0f, y1_base, -3.0f);
	}
	glTranslatef(0.0f, 0.0f, -3.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);

		glVertex3f(x1, 3*y1, 0.0f);
		glVertex3f(x1_base, y1_base, 0.0f);

	glEnd();
	///////////////////////////////////////////////////////////////////////////////////
	// End of Line
	///////////////////////////////////////////////////////////////////////////////////

	x_point_circle -= 0.0030f;
	y_point_circle -= 0.00150f;
	x_point_triangle -= 0.0030f;
	y_point_triangle -= 0.00150f;
	y_point -= 0.0020f;

	angle_rotate += 0.7f;
	if (angle_rotate >= 360.0f)
	{
		angle_rotate = 0.0f;
	}

	glXSwapBuffers(gpDisplay, gWindow);
}


GLfloat DistanceBetweenTwoPoints(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	GLfloat a = x2 - x1;
	GLfloat b = y2 - y1;
	GLfloat distance = sqrt(pow(a, 2) + pow(b, 2));
	distance = distance * 1.0;
	return distance;
}

GLfloat getSidesOfTriangle(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By)
{
	GLfloat DistanceBetweenTwoPoints(GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat distance = 0.0f;
	distance = DistanceBetweenTwoPoints(Ax, Ay, Bx, By);
	return distance;
}

GLfloat getCenter(GLfloat a, GLfloat b, GLfloat c, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat ans = 0.0f;
	ans = ((a * x) + (b * y) + (c * z)) / (a + b + c);
	return ans;
}

// Radius of circle
GLfloat getRadius(GLfloat a, GLfloat b, GLfloat c)
{
	GLfloat s = 0.0f, area = 0.0f;
	s = (a + b + c) / 2.0f;    // Semi-perimeter 
	area = sqrt(s * (s - a) * (s - b) * (s - c));
	return (GLfloat)(area / s);
}


// Midpoint of line
GLfloat getCenterOfLine(GLfloat Ax, GLfloat Bx)
{
	GLfloat ans = (Ax + Bx) / 2.0f;
	return ans;
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