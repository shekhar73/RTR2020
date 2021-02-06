#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

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

int giWindowWidth			= 800;
int giWindowHeight			= 600;

static XFontStruct *pxFontStruct	= NULL;
static GC gc;
XGCValues gcValues;
XColor greenColor;
char str[]	= {"Hello X-Windows From Somshekhar Karle"};
int stringLength;
int stringWidth;
int fontHeight; 

// entry-point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullScreen(void);
	void uninitialize(void);

	//variable declarations
	int winWidth 	= giWindowWidth;
	int winHeight 	=	giWindowHeight;

	//code
	CreateWindow();

	//Message Loop
	XEvent event;
	KeySym keysym;

	while(1)
	{
		XNextEvent(gpDisplay, &event);
		switch(event.type)
		{
			case MapNotify:
                pxFontStruct = XLoadQueryFont(gpDisplay, "fixed");
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

				default:
				break;

			}
			break;

			case ButtonPress:
			switch(event.xbutton.button)
			{
				case 1:		// Left mouse button
					break;
				
				case 2:		// Middle mouse button
					break;
				
				case 3:		// Right mouse button
					break;
				
				case 4:		// Mouse wheel up
					break;

				case 5:	// Mouse wheel down
					break;
			}
			break;

			case MotionNotify:
				break;

			case ConfigureNotify:
				winWidth 	= event.xconfigure.width;
				winHeight 	= event.xconfigure.height;
				break;

			case Expose:
				gc 	= XCreateGC(gpDisplay, gWindow, 0, &gcValues);
				XSetFont(gpDisplay, gc, pxFontStruct->fid);
				XAllocNamedColor(gpDisplay, gColormap, "green", &greenColor, &greenColor);
				XSetForeground(gpDisplay, gc, greenColor.pixel);
				stringLength = strlen(str);
				stringWidth	 = XTextWidth(pxFontStruct, str, stringLength);
				fontHeight   = pxFontStruct->ascent + pxFontStruct->descent;
				XDrawString(gpDisplay, gWindow, gc, (winWidth / 2 - stringWidth / 2), (winHeight / 2 - fontHeight / 2), str, stringLength);
				break;

			case DestroyNotify:
				break;

			case 33:
				XUnloadFont(gpDisplay, pxFontStruct->fid);
				XFreeGC(gpDisplay, gc);
				uninitialize();
				exit(0);

			default:
				break;
		}
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

	//code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL)
	{
		printf("ERROR : Unable to open X Display.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}

	defaultScreen 	= XDefaultScreen(gpDisplay);
	defaultDepth 	= DefaultDepth(gpDisplay, defaultScreen);
	gpXVisualInfo 	= (XVisualInfo *)malloc(sizeof(XVisualInfo));

	if(gpXVisualInfo == NULL)
	{
		printf("ERROR : Unable to Allocate Memory for Visual Info.\nExiting Now...\n");
		uninitialize();
		exit(1);	
	}

	XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
	if(gpXVisualInfo == NULL)
	{
		printf("ERROR : Unable to Get A Visual.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}

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

	XStoreName(gpDisplay, gWindow, "My First Window : SOMSHEKHAR KARLE");

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
