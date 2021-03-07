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

//namespaces
using namespace std;

//global variable declarations
bool bFullScreen			= false;
Display *gpDisplay			= NULL;
XVisualInfo *gpXVisualInfo	= NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;
GLuint textureSmiley;
int pressedDigit;

int giWindowWidth			= 800;
int giWindowHeight			= 600;

// entry-point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullScreen(void);
	void Initialize(void);
	void Uninitialize(void);
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
					Uninitialize();
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

					case 49:
						glEnable(GL_TEXTURE_2D);
						pressedDigit = 1;
						break;

					case 50:
						glEnable(GL_TEXTURE_2D);
						pressedDigit = 2;
						break;

					case 51:
						glEnable(GL_TEXTURE_2D);
						pressedDigit = 3;
						break;

					case 52:
						glEnable(GL_TEXTURE_2D);
						pressedDigit = 4;
						break;

					default:
						glDisable(GL_TEXTURE_2D);
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
	Uninitialize();
	return (0);
}

void CreateWindow(void)
{

	void Uninitialize(void);

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
		Uninitialize();
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
		Uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay, gWindow, "TWEKDED SMILEY : SOMSHEKHAR KARLE");

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

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);

	textureSmiley = LoadBitmapAsTexture("Smily.bmp");
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	Resize(giWindowWidth, giWindowHeight);
}

GLuint LoadBitmapAsTexture(const char* path)
{
	// variable declarations
	int width, height;
	unsigned char* imageData = NULL;
	GLuint textureId;

	// code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	//RGBA
	glGenTextures(1, &textureId);	//Number of textures, texture parameter
	
	glBindTexture(GL_TEXTURE_2D, textureId);
		// Setting texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Following call will actually push the data in to the memory
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	SOIL_free_image_data(imageData);

	return textureId;
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
	// function declaration

	// local variable
	static GLfloat angle = 0.0f;

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);
	glScalef(0.75f, 0.75f, 0.75f);

	glRotatef(angle, 1.0f, 0.0f, 0.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, textureSmiley);

	// front
	if (pressedDigit == 1)
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			// back
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// right side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// left side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			// top side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			// down side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

		glEnd();
	}
	else if (pressedDigit == 2)
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			// back
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// right side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// left side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			// top side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			// down side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

		glEnd();
	}
	else if (pressedDigit == 3)
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			// back
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// right side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// left side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			// top side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			// down side
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

		glEnd();
	}
	else if (pressedDigit == 4)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.5f, 0.5f);

		glBegin(GL_QUADS);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);

		// back
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);

		// right side
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);

		// left side
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		// top side
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);

		// down side
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);

		glEnd();
	}
	else
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);

			// back
			glVertex3f(0.5f, 0.5f, -0.5f);
			glVertex3f(-0.5f, 0.5f, -0.5f);
			glVertex3f(-0.5f, -0.5f, -0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// right side
			glVertex3f(0.5f, 0.5f, -0.5f);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			// left side
			glVertex3f(-0.5f, 0.5f, -0.5f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);
			glVertex3f(-0.5f, -0.5f, -0.5f);

			// top side
			glVertex3f(-0.5f, 0.5f, -0.5f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			// down side
			glVertex3f(-0.5f, -0.5f, -0.5f);
			glVertex3f(-0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, 0.5f);
			glVertex3f(0.5f, -0.5f, -0.5f);

		glEnd();
	}

	angle += 0.2f;
	if(angle >= 360.0f)
		angle = 0.0f;

	glXSwapBuffers(gpDisplay, gWindow);
}

void Uninitialize(void)
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
	if(textureSmiley)
	{
		glDeleteTextures(1, &textureSmiley);
		textureSmiley = 0;
	}
}