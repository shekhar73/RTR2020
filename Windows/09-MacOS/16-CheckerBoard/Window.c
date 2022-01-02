#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> // anyonimous to windows.h
#import <QuartzCore/CVDisplayLink.h>    // Core video-CV
#import <OpenGL/gl3.h>  // like gl.h
#include "vmath.h"

// call back function
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CTimeStamp*, const CVTimeStamp*, CVOptionFlags, CVOptionFlags*, void*);

FILE *gpFile = NULL;

// NS - NeXT Step
// Forward declaration
@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>

@end

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];    // This is called as Run Loop or Message Loop or Event loop
    [pool release];
    return 0;
}

// ApplicationDidFinishLaunching & WMCreate in windows both are anyonimous
// when application gets terminated NSApps chi ajun ek method terminate hote i.e. the application will termiate i.e. WM_Destroy()

// Forward declaration
@interface MyOpenGLView : NSOpenGLView

@end

@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyOpenGLView *myOpenGLView;
}

-(void) ApplicationDidFinishLaunching:(NSNotification *) aNotification
{
    // code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat:@="%@/Log.txt", parentDirPath];
    const char *pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

    gpFile = fopen(pszLogFileNameWithPath, "w");
    if(gpFile == NULL)
    {
        [self release];
        [NSApp terminate:self];
    }

    fprintf(gpFile, "Program Started here...\n");

    // NSRect is internally CGREct
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);   // As this is a C library. First 2 variables are NSPoints i.e. X & Y 3rd and 4th varaibles are width and height i.e. NSSize structure(width and height).
    // NSPoint is internally CGPoint and NSSIze is internally CGSize
    
    window = [[NSWindow alloc]initWithContentRect:win_rect
                styleMask:NSWindowStyleMaskTitled |
                          NSWindowStyleMaskClosable | 
                          NSWindowStyleMaskMiniaturizable | 
                          NSWindowStyleMaskResizable
                backing:NSBackingStoreBuffered
                defer:NO
            ];

    // styleMask = WS_OVERLAPPEDWINDOWNS
    // WindowStyleMaskTitled - WM_CAPTION
    // NSWindowStyleMaskMiniaturizable - WS_MINIMIZE Box
    // NSWindowStyleMaskClosable - WM_CLOSE
    // NSWindowStyleMaskResizable - WM_RESIZE

    [window setTitle:@"SSK : MacOS Window"];
    [window center];
    view = [[MyView alloc] initWithFrame:win_rect];
    [window setContentView:view];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void) ApplicationWillTerminate:(NSNotification *) aNotification
{
    //code
    if(gpFile)
    {
        fprintf(gpFile,"Program Terminated Successfully...\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

-(void) WindowWillClose:(NSNotification *) aNotification
{
    // code
    [NSApp terminate : self];
    
}

-(void) dealloc
{
    // code
    [view release];
    [window release];
    [super dealloc];
}
@end

@implementation MyOpenGLView
{
    @private

        CVDisplayLinkRef displayLink;

        //declare can write our Vao, Vbo and global variables here

    enum
    {
        SSK_ATTRIBUTE_POSITION = 0,
        SSK_ATTRIBUTE_COLOR,
        SSK_ATTRIBUTE_NORMAL,
        SSK_ATTRIBUTE_TEXTURE0
    };

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    // Square
    GLuint gVao_Square;
    GLuint gVbo_Position_Square;
    GLuint gVbo_Texture_Square;

    GLubyte checkImage[CHECK_IMAGE_HEIGHT][CHECK_IMAGE_WIDTH][4];
    GLuint texImage;

    vmath::mat4 gPerspectiveProjectionMatrix;
    GLuint gTextureSamplerUniform;
}

-(id)initWithFrame:(NSRect)initWithFrame
{
    self = [super initWithFrame:Frame];
    if(self)
    {
        NSOpenGLPixelFormatAttribute attributes[] = 
        {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer, 0
        };

        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attributes]autorelease];

        if(pixelFormat == nil)
        {
            fprintf(gpFile, "Can't get Pixel Format...\n");
            [self release];
            [NSApp terminate:self];
        }
        NSOpenGLContext *glContext = [[NSOpenGLContext alloc]]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];
    }
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]int];
    [self drawView];
    [pool release];
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    [super prepareOpenGL];

    [[self openGLContext]makeCurrentContext];

    fprintf(gpFile, "OpenGL Version = %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Swap interval
    GLint swapInt = 1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    ///////////////////////////////////////////////////////////////////////////////////////
    //*** VERTEX SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create Shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // Provide source code to shader
    const GLchar* vertexShaderSourceCode =
        "#version 450 core"                         \
        "\n"                                        \
        "in vec4 vPosition;"                        \
        "in vec2 vTexture;"                         \
        "uniform mat4 u_mvpMatrix;"                 \
        "out vec2 out_texture;"                     \
        "void main(void)"                           \
        "\n"                                        \
        "{"                                         \
        "gl_Position = u_mvpMatrix * vPosition;"    \
        "out_texture = vTexture;"                   \
        "}";

    //gl_Position is inbuilt variable in shader
    // feed the source code to your shaderobject
    glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gVertexShaderObject);

    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;

    // shader compilation error checking

    //void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // glGetShaderiv — return a parameter from a shader object

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

                // take log
                glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                //destroyWindow(ghwnd);
                UnInitialize(); //destroyWindow(ghwnd);-> call WND_DESTROY->PostQuitMessage()->MessageLoop()->uninitialize();
                exit(0);
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    //*** FRAGMENT SHADER ***
    ///////////////////////////////////////////////////////////////////////////////////////

    // Create shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
    const GLchar* fragmentShaderSourceCode =
        "#version 450 core"                                     \
        "\n"                                                    \
        "in vec2 out_texture;"                                  \
        "uniform sampler2D u_texture_sampler;"                  \
        "out vec4 FragColor;"                                   \
        "void main(void)"                                       \
        "\n"                                                    \
        "{"                                                     \
        "FragColor = texture(u_texture_sampler, out_texture);"  \
        "}";

    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gFragmentShaderObject);

    // fragment shader compilation error checking


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
                fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog);
                free(szInfoLog);
                UnInitialize();
                exit(0);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //*** SHADER PROGRAM ***
    /////////////////////////////////////////////////////////////////////////////////////

    // create
    gShaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXTURE, "vTexture");

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");

    // link shader
    glLinkProgram(gShaderProgramObject);

    GLint iShaderProgramLinkStatus = 0;

    // Reset values
    iInfoLogLength = 0;
    szInfoLog = NULL;

    // shader link error checking
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
    // post linking
    gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
    gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    const GLfloat squareVertices[] = {
                                        1.0f, 1.0f,
                                        0.0f, 1.0f,
                                        0.0f, 0.0f,
                                        1.0f, 0.0f
    };



    // It will store all the below 6 steps so that we can reuse it in draw function
    glGenVertexArrays(1, &gVao_Square);
    glBindVertexArray(gVao_Square);

    //OpenGL generates the buffer - OpenGL gives you the buffer symbol to you for bind
    // glGenBuffers(how many buffers create, named buffer object(symbol));
    glGenBuffers(1, &gVbo_Position_Square); //like glGenTextures() in FFP

    // map the buffer symbol(in OpenGL) with my attribute
    
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);    // like glBindTexture() in FFP
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenBuffers(1, &gVbo_Texture_Square);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Square);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutpurCallback(displayLink, &MyDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[self pixelFormat];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(void)LoadGLTexture
{
    // Function declarations
    void MakeCheckImage();

    // code
    MakeCheckImage();
    
    glGenTextures(1, &texImage);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // push data into graphic memory with the help of graphics driver
    glTexImage2D(GL_TEXTURE_2D, 0, 4, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

-(void)MakeCheckImage
{
    int i, j, c;

    for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
    {
        for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            checkImage[i][j][0] = (GLubyte)c;
            checkImage[i][j][1] = (GLubyte)c;
            checkImage[i][j][2] = (GLubyte)c;
            checkImage[i][j][3] = (GLubyte)255;
        }
    }
}

-(void)reshape
{
    [super reshape];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    NSRect rect = [self bounds];
    if(rect.size.height < 0)
    {
        rect.size.height = 1;
    }
    glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
    gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)rect.size.width/(GLfloat)rect.size.width, 0.1f, 100.0f);

    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)drawRect:(NSRect)dirtyRect
{
    // code
    [self drawView];
}

-(void)drawRect
{
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   GLfloat straight[12] = {
             0.0f,  1.0f, 0.0f,
            -2.0f,  1.0f, 0.0f,
            -2.0f, -1.0f, 0.0f,
             0.0f, -1.0f, 0.0f,
    };

    GLfloat angular[12] = {
            2.41421,     1,     -1.41421,
            1,           1,      0,
            1,          -1,      0,
            2.41421,    -1,     -1.41421
    };

    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // start using OpenGL program object
    glUseProgram(gShaderProgramObject);

    // OpenGL Drawing
    // set modelview & modelviewprojection matrices to identity
    mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    mat4 modelViewProjectionMatrix = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();

    // multiply the modelview and perspective matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // ORDER IS IMPORTANT

    // pass above modelviewprojection matrix to the vertex shader in 'u_mvpMatrix' shader variable
    // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glUniform1i(gTextureSamplerUniform, 0);
    

    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), straight, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(gVao_Square);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 (each with its x, y, z) vertices in triangleVertices array

    // *** unbind vao ***
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), angular, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(gVao_Square);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 (each with its x, y, z) vertices in triangleVertices array


    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);   
}

-(BOOL) acceptFirstResponder
{
    // code
    [[self window] makeFirstResponder : self];
    return YES;
}

-(void) keyDown:(NSEvent *) theEvent
{
    // code
    int key = [[theEvent characters] characterAtIndex:0];

    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate : self];
            break;

        case 'F':
        case 'f':
            [[self window] toggleFullScreen:self];
            break;
    }
    
}

-(void) mouseDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Left Mouse Button Is Clicked";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

-(void) rightMouseDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Right Mouse Button Is Clicked";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

-(void) otherButtonDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Hello World Other button down";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

-(void)dealloc
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    [super dealloc];
}

@end

// global
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime:outputTime];
    return (result);
}

