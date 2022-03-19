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

    GLuint gVao;
    GLuint gVbo_position;
    GLuint gMVPUniform;

    // For Tesilation shaders
    GLuint teasilationControlShaderObject;
    GLuint teasilationEvalutionShaderObject;

    // Uniforms for Teasilation Shaders
    GLuint numberOfSegmentsUniform;
    GLuint numberofStripsUniform;
    GLuint lineColorUniform;

    // It is a matrix of 4 X 4 which is declared in vmath.h header file.
    // It will be used for transformation
    //mat4 gPerspectiveProjectionMatrix; // vmath::mat4 gPerspectiveProjectionMatrix
    mat4 gPerspectiveProjectionMatrix;

    // Teasilation shaders
    unsigned int uiNumberOfLineSegments;
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
        "#version 410 core"                             \
        "\n"                                            \
        "in vec4 vPosition;"                            \
        "void main(void)"                               \
        "\n"                                            \
        "{"                                             \
            "gl_Position = vec4(vPosition);"    \
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

    // void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
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
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //*** TEASILATION SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create Shader
    teasilationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

    // Provide source code to shader
    const GLchar* teasilationShaderSourceCode =
        "#version 410 core"                                                                 \
        "\n"                                                                                \
        "layout(vertices = 4)out;"                                                          \
        "uniform int numberOfSegments;"                                                     \
        "uniform int numberOfStrips;"                                                       \
        "void main(void)"                                                                   \
        "\n"                                                                                \
        "{"                                                                                 \
            "gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"     \
            "gl_TessLevelOuter[0] = float(numberOfStrips);"                                 \
            "gl_TessLevelOuter[1] = float(numberOfSegments);"                               \
        "}";

    //gl_Position is inbuilt variable in shader
    // feed the source code to your shaderobject
    glShaderSource(teasilationControlShaderObject, 1, (const GLchar**)&teasilationShaderSourceCode, NULL);

    // compile shader
    glCompileShader(teasilationControlShaderObject);

     iInfoLogLength = 0;
     iShaderCompiledStatus = 0;
     szInfoLog = NULL;

    // shader compilation error checking

    // void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // glGetShaderiv — return a parameter from a shader object

    glGetShaderiv(teasilationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(teasilationControlShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;

                // take log
                glGetShaderInfoLog(teasilationControlShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Teassilation Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    //*** TEASILATION EAVAULATION SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create Shader
    teasilationEvalutionShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

    // Provide source code to shader
    const GLchar* teasilationEvalutaionShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "layout(isolines)in;" \
        "uniform mat4 u_mvp_matrix;"                                    \
        "void main(void)"                                               \
        "\n"                                                            \
        "{"                                                             \
            "float tessCoord = gl_TessCoord.x;"                         \
            "vec3 P0 = gl_in[0].gl_Position.xyz;"                       \
            "vec3 P1 = gl_in[1].gl_Position.xyz;"                       \
            "vec3 P2 = gl_in[2].gl_Position.xyz;"                       \
            "vec3 P3 = gl_in[3].gl_Position.xyz;"                       \
            "vec3 P  = P0 * (1.0 - tessCoord) * (1.0 - tessCoord) * (1.0 - tessCoord) + P1 * 3.0 * tessCoord * (1.0 - tessCoord) * (1.0 - tessCoord) + P2 * 3.0 * tessCoord * tessCoord * (1.0 - tessCoord) + P3 * tessCoord * tessCoord * tessCoord;"  \
            "gl_Position = u_mvp_matrix * vec4(P, 1.0);"                \
        "}";

    //gl_Position is inbuilt variable in shader
    // feed the source code to your shaderobject
    glShaderSource(teasilationEvalutionShaderObject, 1, (const GLchar**)&teasilationEvalutaionShaderSourceCode, NULL);

    // compile shader
    glCompileShader(teasilationEvalutionShaderObject);

     iInfoLogLength = 0;
     iShaderCompiledStatus = 0;
     szInfoLog = NULL;

    // shader compilation error checking

    // void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // glGetShaderiv — return a parameter from a shader object

    glGetShaderiv(teasilationEvalutionShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(teasilationEvalutionShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;

                // take log
                glGetShaderInfoLog(teasilationEvalutionShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Teassilation Evalution Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
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
        "#version 410 core"             \
        "\n"                            \
        "out vec4 FragColor;"           \
        "uniform vec4 lineColor;"       \
        "void main(void)"               \
        "\n"                            \
        "{"                             \
            "FragColor = lineColor;"    \
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

    // attach teasilation control shader to shader program
    glAttachShader(gShaderProgramObject, teasilationControlShaderObject);

    // attach teasilation evalution shader to shader program
    glAttachShader(gShaderProgramObject, teasilationEvalutionShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the 'in' and 'uniform' attributes
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
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                exit(0);
            }
        }

    }

    // get MVP uniform location
    // post linking
    gMVPUniform             = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
    numberOfSegmentsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfSegments");
    numberofStripsUniform   = glGetUniformLocation(gShaderProgramObject, "numberOfStrips");
    lineColorUniform        = glGetUniformLocation(gShaderProgramObject, "lineColor");

    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    const GLfloat vertices[] = {    
                                    -1.0f, -1.0f,
                                    -0.5f,  1.0f,
                                     0.5f, -1.0f,
                                     1.0f,  1.0f
    };

    // It will store all the below 6 steps so that we can reuse it in draw function
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);

    glGenBuffers(1, &gVbo_position);    //like glGenTextures() in FFP

    glBindBuffer(GL_ARRAY_BUFFER, gVbo_position);   // like glBindTexture() in FFP
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);

    uiNumberOfLineSegments = 1;
    // Unbind the BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

   // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    // we will always cull back faces for better performance
    glEnable(GL_CULL_FACE);

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

        case NSKeyDown:
        case 's':
            uiNumberOfLineSegments++;
            if (uiNumberOfLineSegments >= 50)
            {
                uiNumberOfLineSegments = 50;
            }
            break;

        case 'w':
        case NSKeyUp:
            uiNumberOfLineSegments--;
            if (uiNumberOfLineSegments <= 0)
            {
                uiNumberOfLineSegments = 1;

            }
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

    // start using OpenGL program object
    glUseProgram(gShaderProgramObject);

    // OpenGL Drawing // set modelview & modelviewprojection matrices to identity
    mat4 modelViewMatrix = vmath::translate(0.0f,0.0f,-4.0f);
    mat4 modelViewProjectionMatrix = mat4::identity(); //in resize of FFP glMatrixMode(GL_PROJECTION); glLoadIdentity();

    // multiply the modelview and perspective matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // ORDER IS IMPORTANT because Matrix mulitplication is not commutative i.e. (a * b) != (b * a)

    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glUniform1i(numberOfSegmentsUniform, uiNumberOfLineSegments);
    glUniform1i(numberofStripsUniform, 1);
    glUniform4fv(lineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    // *** bind vao *** // all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_PATCHES, 0, 4);

    // *** unbind vao ***
    glBindVertexArray(0);

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
