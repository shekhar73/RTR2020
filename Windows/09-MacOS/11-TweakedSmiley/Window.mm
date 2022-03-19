#import <Foundation/Foundation.h>   //This is like stdio.h
#import <Cocoa/Cocoa.h> // anyonimous to windows.h
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>

#include "vmath.h"

using namespace vmath;

enum
{
    SSK_ATTRIBUTE_POSITION = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXCOORD
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef ,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags , CVOptionFlags *,void *);
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

//ApplicationDidFinishLaunching - WMCreate in windows both are anyonimous
// when application gets terminated NSApps chi ajun ek method terminate hote i.e. the application will termiate i.e. WM_Destroy()

// Forward declaration
@interface MyOpenGLView : NSOpenGLView
@end

@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyOpenGLView * myOpenGLview;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSBundle *appBundle =           [NSBundle mainBundle];
    NSString *appDirectoryPath =    [appBundle bundlePath];
    NSString *parentDirectoryPath = [appDirectoryPath stringByDeletingLastPathComponent];
    NSString *logFile =             [NSString stringWithFormat:@"%@/Log.txt", parentDirectoryPath];
    const char * pzLogFile =        [logFile cStringUsingEncoding:NSASCIIStringEncoding];
    
    gpFile = fopen(pzLogFile,"w");
    if(gpFile == NULL)
    {
        [self release];
        [NSApp terminate:self];
    }
    fprintf(gpFile,"Log file created successfully\n");

    NSRect win_rect = NSMakeRect(0.0,0.0,800.0,600.0);
    window =        [[NSWindow alloc]initWithContentRect:win_rect
                        styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                        backing:NSBackingStoreBuffered
                        defer:NO];
    [window setTitle:@"SSK : Blue Screen"];
    [window center];
    myOpenGLview = [[MyOpenGLView alloc]initWithFrame:win_rect];
    [window setContentView:myOpenGLview];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)aNotification
{
    //code
    // close the file
    if(gpFile){
        fprintf(gpFile,"Log File closed successfully\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

-(void)windowWillClose:(NSNotification *)aNotification
{
    //code
    [NSApp terminate:self];
}

-(void)dealloc
{
    //code
    [myOpenGLview release];
    [window release];
    [super dealloc];

}
@end

@implementation MyOpenGLView
{
    @private
    CVDisplayLinkRef displayLink;

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    // Square
    GLuint gVao_Square;
    GLuint gVbo_Position_Square;
    GLuint gVbo_Texture_Square;

    GLuint gMVPUniform;

    mat4 gPerspectiveProjectionMatrix;

    GLuint smileyTexture;
    GLuint gTextureSamplerUniform;
}


-(id)initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];
    
    if(self){
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAOpenGLProfile,NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask,CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,NSOpenGLPFAColorSize,24,NSOpenGLPFADepthSize,24,NSOpenGLPFAAlphaSize,8,NSOpenGLPFADoubleBuffer,
            0
        };
        
        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attributes]autorelease];
        if(pixelFormat == nil){
            fprintf(gpFile,"ERROR:In getting pixel format\n");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];        
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];        
    }
    
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    //code    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc]init];
    [self drawView];
    [pool release];
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    //code
    GLint swapInt = 1;
    [super prepareOpenGL];
    [[self openGLContext]makeCurrentContext];
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
       
///////////////////////////////////////////////////////////////////////////////////////
    //*** VERTEX SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create Shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // Provide source code to shader
    const GLchar *vertexShaderSourceCode = 
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
    glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

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
                UnInitialize(); //destroyWindow(ghwnd);-> call WND_DESTROY->PostQuitMessage()->MessageLoop()->[self release];
                [NSApp terminate:self];
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    //*** FRAGMENT SHADER ***
    ///////////////////////////////////////////////////////////////////////////////////////

    // Create shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
    const GLchar *fragmentShaderSourceCode = 
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
                [self release];
                [NSApp terminate:self];
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
                [self release];
                [NSApp terminate:self];
            }
        }

    }

    // get MVP uniform location
    // post linking
    gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
    gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    const GLfloat cubeVertices[] = {
                                         1.0f,  1.0f, 0.0f,
                                        -1.0f,  1.0f, 0.0f,
                                        -1.0f, -1.0f, 0.0f,
                                         1.0f, -1.0f, 0.0f,             
    };

    const GLfloat cubeTextures[] = {
                                        1.0f, 1.0f,
                                        0.0f, 1.0f,
                                        0.0f, 0.0f,
                                        1.0f, 0.0f,
    };



    // It will store all the below 6 steps so that we can reuse it in draw function
    glGenVertexArrays(1, &gVao_Square);
    glBindVertexArray(gVao_Square);

    //OpenGL generates the buffer - OpenGL gives you the buffer symbol to you for bind
    // glGenBuffers(how many buffers create, named buffer object(symbol));
    glGenBuffers(1, &gVbo_Position_Square); //like glGenTextures() in FFP

    // map the buffer symbol(in OpenGL) with my attribute
    // glBindBuffer(
    //               which type of buffer(like position, color, texcord, normal) => GL_ARRAY_BUFFER,
    //               in which symbol I'll add this BUFFER_ARRAY
    //             );
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);    // like glBindTexture() in FFP
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // tell how to use the data which is added in GL_ARRAY_BUFFER
    // glVertexAttribPointer(where I have to add in shader,
    //                          split the triangeVertices array in 3 parts(total 9 vertices) bcz it has 3 vertices i.e. (x, y, z)
    //                          type of data which is sent,
    //                          can I make it NORMALIZE or not(fit it in -1 to 1(normalize),
    //                          kitine dhanga taku data ghyayla,    
    //                          kitine udya maru(udaych maraych nahi so NULL
    //                      );

    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    // Unbind the BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenBuffers(1, &gVbo_Texture_Square);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Square);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextures), cubeTextures, GL_STATIC_DRAW);
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
   
    smileyTexture    = [self loadGLTextures:"Smiley.bmp"];

    // set background color to which it will display even if it will empty.
    // THIS LINE CAN BE IN drawRect().
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);   // blue

    gPerspectiveProjectionMatrix = mat4::identity();

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}



-(GLuint)loadGLTextures:(const char*)imageFileName
{
    //code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *imageFileNameWithPath = [NSString stringWithFormat:@"%@/%s",parentDirPath,imageFileName];
    NSImage *bmpImage = [[NSImage alloc]initWithContentsOfFile:imageFileNameWithPath];

    if(!bmpImage){
        fprintf(gpFile,"NSImage conversion of image file failed\n");
        return(0);
    }

    CGImageRef CGImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];
        
    int imageWidth  = (int)CGImageGetWidth(CGImage);
    int imageHeight = (int)CGImageGetHeight(CGImage);
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(CGImage));
        
    void *pixel = (void*)CFDataGetBytePtr(imageData);
    
    GLuint texture;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    CFRelease(imageData);
    return(texture); 
}


// dirtyRect - InvalidateRect

-(void)drawRect:(NSRect)dirtyRect
{

}

-(void)reshape
{
    //code
    [super reshape];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    NSRect rect = [self bounds];
    
    if(rect.size.height <= 0)
    {
        rect.size.height = 1;
    }
    
    glViewport(0,0,(GLsizei)rect.size.width,(GLsizei)rect.size.height);
    gPerspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)rect.size.width / (GLfloat)rect.size.height), 0.1f, 100.0f);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(BOOL)acceptsFirstResponder
{
    //code
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent*)theEvent
{
    //code
    int key = [[theEvent characters]characterAtIndex:0];
    
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate:self];
            break;
            
        case 'f':
        case 'F':
            [[self window]toggleFullScreen:self];
            break;
    }
}


-(void)drawView
{
    //code
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static GLfloat texCord[8];

    mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // start using OpenGL program object
    glUseProgram(gShaderProgramObject);

    // OpenGL Drawing
    // set modelview & modelviewprojection matrices to identity
    mat4 modelViewMatrix = TranslateMatrix;
    mat4 modelViewProjectionMatrix = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();

    // multiply the modelview and perspective matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // ORDER IS IMPORTANT

    // pass above modelviewprojection matrix to the vertex shader in 'u_mvpMatrix' shader variable
    // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    
    //glUniformMatrix4fv = gl-> OpenGL
    //Uniform -> from initialize
    //  Matrix -> matrix
    //  4 -> 4 X 4 matrix
    //  f -> float
    //  v -> array (vector)
    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smileyTexture);
    glUniform1i(gTextureSamplerUniform, 0);

    // *** bind vao ***
    // all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao_Square);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 (each with its x, y, z) vertices in triangleVertices array

    // *** unbind vao ***
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Stop using OpenGL program object
    glUseProgram(0);


    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}


-(void)dealloc
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    [super dealloc];
}
@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *now,const CVTimeStamp *outputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut,void* displayLinkContext)
{    
    CVReturn result = [(MyOpenGLView*)displayLinkContext getFrameForTime:outputTime]; 
    return(result);
}