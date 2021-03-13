#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "Teapot.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

bool bTexture = false;
bool bLight = false;

//namespaces
using namespace std;

//global variable declarations
bool bFullScreen			= false;
Display *gpDisplay			= NULL;
XVisualInfo *gpXVisualInfo	= NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;
GLuint marbleTexture;

int giWindowWidth			= 800;
int giWindowHeight			= 600;

GLUquadric* quadric = NULL;

GLfloat lightAmbiant[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// grey light
GLfloat lightDefuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };	// light from +z to -z

GLfloat materialAmbiant[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// grey light
GLfloat materialDefuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// white light
GLfloat materialShinyness = 128.0f;

static GLfloat angle = 0.0f;

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

					case 'A':
					case 'a':
						angle += 1.0f;
						if (angle == 360.0f)
							angle = 0.0f;
					break;

					case 'T':
					case 't':
						if (bTexture == false)
						{
							glEnable(GL_TEXTURE_2D);
							bTexture = true;
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
							bTexture = false;
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
	GLuint LoadBitmapAsTexture(const char*);

	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	marbleTexture = LoadBitmapAsTexture("Marble.bmp");

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbiant);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDefuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbiant);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShinyness);

	glEnable(GL_LIGHT0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	Resize(giWindowWidth, giWindowHeight);
}

GLuint LoadBitmapAsTexture(const char* path)
{
	// Variable declarations
	int width, height;
	unsigned char* imageData = NULL;
	GLuint textureID;

	// code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	SOIL_free_image_data(imageData);

	return textureID;
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
	
	glTranslatef(0.0f, 0.0f, -2.0f);
	glBindTexture(GL_TEXTURE_2D, marbleTexture);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	for(int i = 0; i < sizeof(face_indices) / sizeof(face_indices[0]); i++)
	{
		for(int j = 0; j < 3; j++)
		{
			int vi = face_indices[i][j];
			int ni = face_indices[i][j + 3];
			int ti = face_indices[i][j + 6];

			glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);
			glTexCoord2f(textures[ti][0], textures[ti][1]);
			glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
		}
	}

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