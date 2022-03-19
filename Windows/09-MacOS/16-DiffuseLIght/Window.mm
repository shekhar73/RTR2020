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

    // cube
    GLuint gVao_Cube;
    GLuint gVbo_Position_Cube;
    GLuint gVbo_Cube_Normal;

    GLuint modelViewMatrixUniform;
    GLuint projectionMatrix;
    GLuint LKeyPressedUniform;
    GLuint LdUniform;                       // Diffuese component of light
    GLuint KdUniform;                       // material diffuse component of light
    GLuint LightPositionUniform;

    bool bAnimate;
    bool bLight;

    mat4 gPerspectiveProjectionMatrix;
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
       

/////////////////////////////////////////////////////////////////////////////////
    //vertex shader
    /////////////////////////////////////////////////////////////////////////////////

    // create shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    // 1. Calculate Eye cordinate
        // 2. Calculate Normal matrix
        // 3. Calculate Transformed Normals
        // 4. Calculate s Source of Light
        // 5. Calculate Diffuse Light by using light equation

        // Eye coordinates are calculated by multiplying position coordinates and model view matrix
        // Normal matrix is inverse of Transpose of upper 3 X 3 of Model View Matrix

    //provide source code to shader
    const GLchar* vertexShaderSourceCode =
        "#version 450 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_model_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_l_key_preesed;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec4 u_light_position;" \
        "out vec3 diffuse_light;" \
        "void main(void)" \
        "{" \
        "if(u_l_key_preesed == 1)" \
        "{" \
        "vec4 eye_coordinates = u_model_view_matrix * vPosition;" \
        "mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));" \
        "vec3 t_norm = normalize(normal_matrix * vNormal);" \
        "vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
        "diffuse_light = u_ld * u_kd * max(dot(s, t_norm), 0.0f);" \
        "}"                                                                             \
        "gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;"          \
        "}";

    glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gVertexShaderObject);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;

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
                glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    //Fragment shader
    /////////////////////////////////////////////////////////////////////////////////

    // crete shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
    const GLchar* fragmentShaderSourceCode =
        "#version 450 core"                                     \
        "\n"                                                    \
        "in vec3 diffuse_light;"                                \
        "uniform int u_l_key_preesed;"                          \
        "out vec4 FragColor;"                                   \
        "vec4 color;"                                           \
        "void main(void)"                                       \
        "\n"                                                    \
        "{"                                                     \
        "if(u_l_key_preesed == 1)"                              \
        "{"                                                     \
        "color = vec4(diffuse_light, 1.0f);"                    \
        "}"                                                     \
        "else"                                                  \
        "{"                                                     \
        "color = vec4(1.0, 1.0f, 1.0f, 1.0f);"                  \
        "}"                                                     \
        "FragColor = color;"                                    \
        "}";

    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    // compile shader
    glCompileShader(gFragmentShaderObject);

    // Error checking
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
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    // shader program
    /////////////////////////////////////////////////////////////////////////////////

    // create program
    gShaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");


    // link shader
    glLinkProgram(gShaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;

    // Link error checking
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
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    modelViewMatrixUniform  = glGetUniformLocation(gShaderProgramObject, "u_model_view_matrix");
    projectionMatrix        = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
    LKeyPressedUniform      = glGetUniformLocation(gShaderProgramObject, "u_l_key_preesed");
    LdUniform               = glGetUniformLocation(gShaderProgramObject, "u_ld");
    KdUniform               = glGetUniformLocation(gShaderProgramObject, "u_kd");
    LightPositionUniform    = glGetUniformLocation(gShaderProgramObject, "u_light_position");

    // vertices, colors. shader attribs, vbo, vao, initializations
    const GLfloat cubeVertices[] = {
         // Front side
         0.5f,  0.5f, 0.5f,
        -0.5f,  0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
         0.5f, -0.5f, 0.5f,

         // Top side
         0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f,  0.5f,
         0.5f, 0.5f,  0.5f,

         // Back side
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,


         // Bottom side
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

         // Right side
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,

         // Left side
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,

    };

    const GLfloat cubeNormals[] = {
        0.0f, 0.0f, 1.0f,       // Front side
        0.0f, 0.0f, 1.0f,       // Front side
        0.0f, 0.0f, 1.0f,       // Front side
        0.0f, 0.0f, 1.0f,       // Front side

        0.0f, 1.0f, 0.0f,       // Top side
        0.0f, 1.0f, 0.0f,       // Top side
        0.0f, 1.0f, 0.0f,       // Top side
        0.0f, 1.0f, 0.0f,       // Top side

        0.0f, 0.0f, -1.0f,      // Back side
        0.0f, 0.0f, -1.0f,      // Back side
        0.0f, 0.0f, -1.0f,      // Back side
        0.0f, 0.0f, -1.0f,      // Back side

        0.0f, -1.0f, 0.0f,      // Bottom side
        0.0f, -1.0f, 0.0f,      // Bottom side
        0.0f, -1.0f, 0.0f,      // Bottom side
        0.0f, -1.0f, 0.0f,      // Bottom side

        1.0f, 0.0f, 0.0f,       // Right side
        1.0f, 0.0f, 0.0f,       // Right side
        1.0f, 0.0f, 0.0f,       // Right side
        1.0f, 0.0f, 0.0f,       // Right side

        -1.0f, 0.0f, 0.0f,      // Left side
        -1.0f, 0.0f, 0.0f,      // Left side
        -1.0f, 0.0f, 0.0f,      // Left side
        -1.0f, 0.0f, 0.0f,      // Left side


    };

    glGenVertexArrays(1, &gVao_Cube);
    glBindVertexArray(gVao_Cube);

    // Vbo Position Cube
    glGenBuffers(1, &gVbo_Position_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Vbo Normal Cube
    glGenBuffers(1, &gVbo_Cube_Normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Cube_Normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(0);

   // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);   // blue


    bAnimate = false;
    bLight = false;

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

        case 'A':
        case 'a':
            if(bAnimate == true)
            {
                bAnimate = false;
            }
            else
            {
                bAnimate = true;
            }
            break;
            
        case 'L':
        case 'l':
            if(bLight == false)
            {
                bLight  = true;
            }
            else
            {
                bLight = false;
            }
            break;
    }
}


-(void)drawView
{
    //code
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   static GLfloat cubeAngle = 0.0f;

    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // start using opengl program object
    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        GLfloat lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

        glUniform1i(LKeyPressedUniform, 1);
        glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f);                         
        glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f);                           
        glUniform4fv(LightPositionUniform, 1, (GLfloat*)lightPosition);     
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    mat4 modelViewMatrix = mat4::identity();
    mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

    mat4 RotationMatrix1 = vmath::rotate((GLfloat)cubeAngle, 1.0f, 1.0f, 0.0f);
    mat4 RotationMatrix2 = vmath::rotate((GLfloat)cubeAngle, 0.0f, 1.0f, 0.0f);
    mat4 RotationMatrix3 = vmath::rotate((GLfloat)cubeAngle, 0.0f, 0.0f, 1.0f);

    modelViewMatrix = TranslateMatrix * RotationMatrix1 * RotationMatrix2 * RotationMatrix3;

    glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, gPerspectiveProjectionMatrix);

    // bind vao
    glBindVertexArray(gVao_Cube);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

    // unbind vao
    glBindVertexArray(0);

    // stop using opengl program object
    glUseProgram(0);

    if (bAnimate == true)
    {
        cubeAngle = cubeAngle + 0.1f;
        if (cubeAngle >= 360.0f)
        {
            cubeAngle = 0.0f;
        }
    }

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