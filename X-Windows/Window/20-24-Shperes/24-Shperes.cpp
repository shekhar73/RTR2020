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

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

// Axis Rotation
GLfloat anlgeForXRotataion = 0.0f;
GLfloat anlgeForYRotataion = 0.0f;
GLfloat anlgeForZRotataion = 0.0f;

// Quadric for  24 sphere
GLUquadric* quadric[24];
GLint keyPressed = 0;

GLfloat lightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };		// Light is coming from outside of stage i.e. z-axis It is a Positional Light

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

					case 'x':
					case 'X':
						keyPressed = 1;
						anlgeForXRotataion = 0.0f;
						break;

					case 'y':
					case 'Y':
						keyPressed = 2;
						anlgeForYRotataion = 0.0f;
						break;

					case 'z':
					case 'Z':
						keyPressed = 3;
						anlgeForZRotataion = 0.0f;
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

	//set clear color
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);

	//////////////////////////////////////////////////////////////////
	// Initialization of light
	/////////////////////////////////////////////////////////////////

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++)
	{
		quadric[i] = gluNewQuadric();
	}

	//////////////////////////////////////////////////////////////////
	// Material Initialization
	//////////////////////////////////////////////////////////////////

	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
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
	// Local Function Declaration
	void draw24Sphere(void);

	if (keyPressed == 1)
	{
		glRotatef(anlgeForXRotataion, 1.0f, 0.0f, 0.0f);
		lightPosition[2] = anlgeForXRotataion;
	}
	else if (keyPressed == 2)
	{
		glRotatef(anlgeForYRotataion, 0.0f, 1.0f, 0.0f);
		lightPosition[2] = anlgeForYRotataion;
	}
	else if (keyPressed == 3)
	{
		glRotatef(anlgeForZRotataion, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = anlgeForZRotataion;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	draw24Sphere();

	glXSwapBuffers(gpDisplay, gWindow);
}


void draw24Sphere()
{

	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShineness;;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glTranslatef(6.0f, -3.0f, -30.0f);

	//////////////////////////////////////////////////////////
	// Emerald
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 1.0215f;
	materialAmbient[1] = 0.1745f;
	materialAmbient[2] = 0.0215f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 1.07568f;
	materialDiffuse[1] = 0.61424f;
	materialDiffuse[2] = 0.07568f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.633f;
	materialSpecular[1] = 0.727811f;
	materialSpecular[2] = 0.633f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.6f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 10.0f, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// Jade
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.135f;
	materialAmbient[1] = 0.2225f;
	materialAmbient[2] = 0.1575f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.54f;
	materialDiffuse[1] = 0.89f;
	materialDiffuse[2] = 0.63f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.316228f;
	materialSpecular[1] = 0.316228f;
	materialSpecular[2] = 0.316228f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.1f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-10.0f, 7.50f, 0.0f);

	gluSphere(quadric[1], 1.0f, 30, 30);



	//////////////////////////////////////////////////////////
	// obsidian
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.05375f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.06625f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.18275f;
	materialDiffuse[1] = 0.17f;
	materialDiffuse[2] = 0.22525f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.332741f;
	materialSpecular[1] = 0.328634f;
	materialSpecular[2] = 0.346435f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.3f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 5.0f, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// pearl
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.20725f;
	materialAmbient[2] = 0.20725f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 1.0f;
	materialDiffuse[1] = 0.829f;
	materialDiffuse[2] = 0.829f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.296648f;
	materialSpecular[1] = 0.296648f;
	materialSpecular[2] = 0.296648f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);


	materialShineness = 0.088f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 2.50f, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// ruby
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.1745f;
	materialAmbient[1] = 0.01175f;
	materialAmbient[2] = 0.01175f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.61424f					;
	materialDiffuse[1] = 0.04136f;
	materialDiffuse[2] = 0.04136f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.727811f;
	materialSpecular[1] = 0.626959f;
	materialSpecular[2] = 0.626959f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);


	materialShineness = 0.6f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, 0.0f, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// turquoise
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.1f;
	materialAmbient[1] = 0.18725f;
	materialAmbient[2] = 0.1745f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.396f;
	materialDiffuse[1] = 0.74151f;
	materialDiffuse[2] = 0.69102f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.297254f;
	materialSpecular[1] = 0.30829f;
	materialSpecular[2] = 0.306678f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.1f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-10.0f, -2.5f, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// brass
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.329412f;
	materialAmbient[1] = 0.223529f;
	materialAmbient[2] = 0.027451f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.780392;
	materialDiffuse[1] = 0.568627;
	materialDiffuse[2] = 0.113725;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.992157f;
	materialSpecular[1] = 0.941176f;
	materialSpecular[2] = 0.807843f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.21794872f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-7.5f, 10.0f, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// bronze 
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.2125f;
	materialAmbient[1] = 0.1275f;
	materialAmbient[2] = 0.054f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.714f;
	materialDiffuse[1] = 0.4284f;
	materialDiffuse[2] = 0.18144f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.393548f;
	materialSpecular[1] = 0.271906f;
	materialSpecular[2] = 0.166721f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.2f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-7.5f, 7.50f, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// chrome  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.25f;
	materialAmbient[1] = 0.25f;
	materialAmbient[2] = 0.25f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.4f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.774597f;
	materialSpecular[1] = 0.774597f;
	materialSpecular[2] = 0.774597f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.6f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-7.5, 5.0f, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);

	//////////////////////////////////////////////////////////
	// copper  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.19125f;
	materialAmbient[1] = 0.0735f;
	materialAmbient[2] = 0.0225f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.7038f;
	materialDiffuse[1] = 0.27048f;
	materialDiffuse[2] = 0.0828f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.256777f;
	materialSpecular[1] = 0.137622f;
	materialSpecular[2] = 0.086014f;
	materialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.1f  * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-7.5, 2.50f, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);



	//////////////////////////////////////////////////////////
	// gold  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.24725f;
	materialAmbient[1] = 0.1995f;
	materialAmbient[2] = 0.0745f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.75164f;
	materialDiffuse[1] = 0.60648f;
	materialDiffuse[2] = 0.22648f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.628281f;
	materialSpecular[1] = 0.555802f;
	materialSpecular[2] = 0.366065f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.4f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-7.5, 0.0f, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// silver  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.19225f;
	materialAmbient[1] = 0.19225f;
	materialAmbient[2] = 0.19225f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.50754f;
	materialDiffuse[1] = 0.50754f;
	materialDiffuse[2] = 0.50754f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.508273f;
	materialSpecular[1] = 0.508273f;
	materialSpecular[2] = 0.508273f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.4f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-7.5, -2.50f, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);


	// 3rd Coloumn
	//////////////////////////////////////////////////////////
	// black  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.01f;
	materialDiffuse[1] = 0.01f;
	materialDiffuse[2] = 0.01f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.50f;
	materialSpecular[1] = 0.50f;
	materialSpecular[2] = 0.50f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 10.0f, 0.0f);
	gluSphere(quadric[12], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// cyan  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.1f;
	materialAmbient[2] = 0.06f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.0f;
	materialDiffuse[1] = 0.50980392f;
	materialDiffuse[2] = 0.50980392f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.50196078f;
	materialSpecular[1] = 0.50196078f;
	materialSpecular[2] = 0.50196078f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-5.0, 7.50f, 0.0f);
	gluSphere(quadric[13], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// green  
	//////////////////////////////////////////////////////////
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.1f;
	materialDiffuse[1] = 0.35f;
	materialDiffuse[2] = 0.1f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.45f;
	materialSpecular[1] = 0.55f;
	materialSpecular[2] = 0.45f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 5.0f, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);


	//////////////////////////////////////////////////////////
	// red  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.0f;
	materialDiffuse[2] = 0.0f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.6f;
	materialSpecular[2] = 0.6f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 2.50f, 0.0f);
	gluSphere(quadric[15], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// white  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.55f;
	materialDiffuse[1] = 0.55f;
	materialDiffuse[2] = 0.55f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.70f;
	materialSpecular[1] = 0.70f;
	materialSpecular[2] = 0.70f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-5.0, 0.0f, 0.0f);
	gluSphere(quadric[16], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// yellow plastic  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.60f;
	materialSpecular[1] = 0.60f;
	materialSpecular[2] = 0.50f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.25f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-5.0, -2.50f, 0.0f);
	gluSphere(quadric[17], 1.0, 30, 30);

	/// 4th coloumn
	//////////////////////////////////////////////////////////
	// black  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.02f;
	materialAmbient[1] = 0.02f;
	materialAmbient[2] = 0.02f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.01f;
	materialDiffuse[1] = 0.01f;
	materialDiffuse[2] = 0.01f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.4f;
	materialSpecular[1] = 0.4f;
	materialSpecular[2] = 0.4f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-2.50, 10.0f, 0.0f);
	gluSphere(quadric[18], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// cyan  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);


	materialDiffuse[0] = 0.4f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);



	materialSpecular[0] = 0.04f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.7f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);


	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-2.50, 7.50f, 0.0f);
	gluSphere(quadric[19], 1.0, 30, 30);


	//////////////////////////////////////////////////////////
	// green  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4f; ;
	materialDiffuse[1] = 0.5f; ;
	materialDiffuse[2] = 0.4f; ;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);


	materialSpecular[0] = 0.04f;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-2.50, 5.0f, 0.0f);
	gluSphere(quadric[20], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// red  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.4f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.04f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-2.50, 2.50f, 0.0f);
	gluSphere(quadric[21], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// white  
	//////////////////////////////////////////////////////////

	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.05f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.5f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.7f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-2.50, 0.0f, 0.0f);
	gluSphere(quadric[22], 1.0, 30, 30);

	//////////////////////////////////////////////////////////
	// yellow rubber 
	//////////////////////////////////////////////////////////
	materialAmbient[0] = 0.05f;
	materialAmbient[1] = 0.05f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5f;
	materialDiffuse[1] = 0.5f;
	materialDiffuse[2] = 0.4f;
	materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7f;
	materialSpecular[1] = 0.7f;
	materialSpecular[2] = 0.04f;
	materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	materialShineness = 0.078125f * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, materialShineness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-2.50, -2.50f, 0.0f);
	gluSphere(quadric[23], 1.0, 30, 30);

	anlgeForXRotataion = anlgeForXRotataion + 1.1f;
	anlgeForYRotataion = anlgeForYRotataion + 1.1f;
	anlgeForZRotataion = anlgeForZRotataion + 1.1f;
	
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