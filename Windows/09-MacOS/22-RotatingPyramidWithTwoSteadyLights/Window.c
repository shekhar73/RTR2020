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

    // Light
    struct lightUniform
    {
        GLuint LaUniform;
        GLuint LdUniform;
        GLuint LsUniform;
        GLuint LpUniform;
    }lUniform[2];

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    // pyramid
    GLuint gVao_Pyramid;
    GLuint gVbo_Position_Pyramid;
    GLuint gVbo_Pyramid_Normal;

    GLuint gModelMatrix;
    GLuint gViewMatrix;
    GLuint gProjectionMatrix;

    // Material
    GLuint KdUniform;   // difuse material
    GLuint KaUniform;   // materail of ambient
    GLuint KsUniform;   // specular material

    GLfloat materialShinenessUniform;
    GLuint LKeyPressedUniformUniform;

    bool bLight;
    GLfloat pyramidAngle = 0.0f;

    struct Light
    {
        vec4 light_ambient_color;
        vec4 light_diffuse_color;
        vec4 light_specular_color;
        vec4 light_position;
    }light[2];

    vmath::mat4 gPerspectiveProjectionMatrix;
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

    //vertex shader
    // create shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //provide source code to shader
    const GLchar* vertexShaderSourceCode =
        "#version 450 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform int u_l_key_preesed;" \
        "uniform vec3 u_la[2];" \
        "uniform vec3 u_ld[2];" \
        "uniform vec3 u_ls[2];" \
        "uniform vec4 u_light_position[2];" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ks;" \
        "uniform float u_material_shineness[2];" \
        "out vec3 fong_ads_light;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "void main(void)" \
        "{" \
        "fong_ads_light = vec3(0.0f, 0.0f, 0.0f);"
        "if(u_l_key_preesed == 1)" \
        "{" \
        "int i;" \
        "vec3 light_direction[2], reflection_vector[2], ambient_light[2], diffuse_light[2], specular[2];"   \
        "vec4 eye_coordinates       = u_viewMatrix * u_modelMatrix * vPosition;" \
        "vec3 transformed_normal    = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
        "vec3 view_vector           = normalize(-eye_coordinates.xyz);" \
        "for(i = 0; i < 2; i++)"    \
        "{" \
        "light_direction[i]     = normalize(vec3(u_light_position[i] - eye_coordinates));" \
        "reflection_vector[i]       = reflect(-light_direction[i], transformed_normal);" \
        "ambient_light[i]           = u_la[i] * u_ka;" \
        "diffuse_light[i]           = u_ld[i] * u_kd * max(dot(light_direction[i], transformed_normal), 0.0f);" \
        "specular[i]                = u_ls[i] * u_ks * pow(max(dot(reflection_vector[i], view_vector), 0.0f), u_material_shineness[i]);" \
        "fong_ads_light             = fong_ads_light + ambient_light[i] + diffuse_light[i] + specular[i];" \
        "}" \
        "}" \
        "else"  \
        "{" \
        "fong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix  * vPosition;"  \
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
                UnInitialize();
                exit(0);
            }
        }
    }

    // Fragment shader
    // crete shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
    const GLchar* fragmentShaderSourceCode =
        "#version 450 core" \
        "\n"    \
        "in vec3 fong_ads_light;" \
        "out vec4 FragColor;"   \
        "void main(void)"   \
        "\n"    \
        "{"     \
        "FragColor = vec4(fong_ads_light, 1.0f);" \
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
                UnInitialize();
                exit(0);
            }
        }
    }

    // shader program
    // create program
    gShaderProgramObject = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    //glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");

    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNoraml");

    // link shader
    glLinkProgram(gShaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
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

    // get Uniform location

    // get MVP uniform location
    // post linking


    gModelMatrix = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");   
    gViewMatrix = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");   
    gProjectionMatrix = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");  
    lUniform[0].LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la[0]");  
    lUniform[0].LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld[0]");
    lUniform[0].LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls[0]");  
    lUniform[0].LpUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position[0]");   
    lUniform[1].LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la[1]");  
    lUniform[1].LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld[1]");  
    lUniform[1].LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls[1]");   
    lUniform[1].LpUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position[1]");
    KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    materialShinenessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shineness");
    LKeyPressedUniformUniform = glGetUniformLocation(gShaderProgramObject, "u_l_key_preesed");
    

    // vertices, colors. shader attribs, vbo, vao, initializations
    const GLfloat pyramidVertices[] = {

        // front side
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,

        // left side
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,

        // back side
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // right side
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f
    };

    const GLfloat pyramidNormals[] = { 0.0f, 0.447214f, 0.894427f,
                                        0.0f, 0.447214f, 0.894427f,
                                        0.0f, 0.447214f, 0.894427f,

                                        -0.894427f, 0.447214f, 0.0f,
                                        -0.894427f, 0.447214f, 0.0f,
                                        -0.894427f, 0.447214f, 0.0f,

                                        0.0f, 0.447214f, -0.894427f,
                                        0.0f, 0.447214f, -0.894427f,
                                        0.0f, 0.447214f, -0.894427f,

                                        0.894427f, 0.447214f, 0.0f,
                                        0.894427f, 0.447214f, 0.0f,
                                        0.894427f, 0.447214f, 0.0f };

    glGenVertexArrays(1, &gVao_Pyramid);
    glBindVertexArray(gVao_Pyramid);
    {
        // position vbo
        glGenBuffers(1, &gVbo_Position_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // normal vbo
        glGenBuffers(1, &gVbo_Pyramid_Normal);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Pyramid_Normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutpurCallback(displayLink, &MyDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[self pixelFormat];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
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

    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // start using opengl program object
    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        glUniform1i(LKeyPressedUniformUniform, 1);

        GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat materialDifuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat materialShinyness = 128.0f;

        light[0].light_ambient_color = { 0.0f, 0.0f, 0.0f, 1.0f };
        light[0].light_diffuse_color = { 1.0f, 0.0f, 0.0f, 1.0f };
        light[0].light_specular_color = { 1.0f, 0.0f, 0.0f, 1.0f };
        light[0].light_position = { -5.0f, 0.0f, 0.0f, 1.0f };

        light[1].light_ambient_color = { 0.0f, 0.0f, 0.0f, 1.0f };
        light[1].light_diffuse_color = { 0.0f, 0.0f, 1.0f, 1.0f };
        light[1].light_specular_color = { 0.0f, 0.0f, 1.0f, 1.0f };
        light[1].light_position = { 5.0f, 0.0f, 0.0f, 1.0f };

        glUniform3fv(lUniform[0].LaUniform, 1, light[0].light_ambient_color);
        glUniform3fv(lUniform[0].LdUniform, 1, light[0].light_diffuse_color);
        glUniform3fv(lUniform[0].LsUniform, 1, light[0].light_specular_color);
        glUniform4fv(lUniform[0].LpUniform, 1, light[0].light_position);

        glUniform3fv(lUniform[1].LaUniform, 1, light[1].light_ambient_color);
        glUniform3fv(lUniform[1].LdUniform, 1, light[1].light_diffuse_color);
        glUniform3fv(lUniform[1].LsUniform, 1, light[1].light_specular_color);
        glUniform4fv(lUniform[1].LpUniform, 1, light[1].light_position);

        glUniform3fv(KaUniform, 1, (GLfloat*)materialAmbient);
        glUniform3fv(KdUniform, 1, (GLfloat*)materialDifuse);
        glUniform3fv(KsUniform, 1, (GLfloat*)materialSpecular);
        glUniform1f(materialShinenessUniform, materialShinyness);
    }
    else
    {
        glUniform1i(LKeyPressedUniformUniform, 0);
    }


    /////////////////////////////////////////////////////////////////////////////////////
    // PYRAMID
    /////////////////////////////////////////////////////////////////////////////////////

    mat4 IdentityMatrix = mat4::identity();

    mat4 modelMatrix = IdentityMatrix;
    mat4 viewMatrix = IdentityMatrix;
    mat4 projectionMatrix = IdentityMatrix;
    mat4 translateMatrix = IdentityMatrix;
    mat4 rotationMatrix = IdentityMatrix;
    mat4 scaleMatrix = IdentityMatrix;
    mat4 modelViewProjectionMatrix = IdentityMatrix;

    translateMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);

    modelMatrix = translateMatrix * scaleMatrix * rotationMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;


    //pass above modelviewprojection matrix to the vertex shader in 'u_mvpMatrix' shader variable
    // whose position value we already calculated in intiWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(gModelMatrix, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrix, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gProjectionMatrix, 1, GL_FALSE, projectionMatrix);

    // bind vao
    glBindVertexArray(gVao_Pyramid);
    {
        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        glDrawArrays(GL_TRIANGLES, 0, 12); // 12 total vertex
    }
    glBindVertexArray(0); // unbind vao

    // stop using opengl program object
    glUseProgram(0);

    pyramidAngle += 0.1f;

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

        case 'L':
        case 'l':
            if (bLight == true)
            {
                bLight = false;
            }
            else
            {
                bLight = true;
            }
            break;

        default:
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

