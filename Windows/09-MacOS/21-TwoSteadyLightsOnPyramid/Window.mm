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

        
    bool gbActiveWindow = false;
    bool gbEscapeKeyIsPressed = false;
    bool gbFullScreen = false;

    bool bLight;

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLfloat lightPosition_Red[] = {-2.0f,0.0,0.0f,1.0f};
    GLfloat lightAmbient_Red[] = {0.0f,0.0f,0.0f,1.0f};
    GLfloat lightDiffuse_Red[] = {1.0f,0.0f,0.0f,1.0f};
    GLfloat lightSpecular_Red[] = {1.0f,0.0f,0.0f,1.0f};

    GLfloat lightPosition_Blue[] = {2.0f,0.0,0.0f,1.0f};
    GLfloat lightAmbient_Blue[] = {0.0f,0.0f,0.0f,1.0f};
    GLfloat lightDiffuse_Blue[] = {0.0f,0.0f,1.0f,1.0f};
    GLfloat lightSpecular_Blue[] = {0.0f,0.0f,1.0f,1.0f};

    GLfloat materialAmbient[] = {0.0f,0.0f,0.0f,0.0f};
    GLfloat materialDiffuse[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat materialSpecular[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat materialShininess = 128.0f;

    // pyramid
    GLuint gVao_Pyramid;
    GLuint gVbo_Position_Pyramid;
    GLuint gVbo_Normal_Pyramid;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint perspectiveProjectionUniform;
    GLuint LaUniform_Red;
    GLuint LdUniform_Red;
    GLuint LsUniform_Red;
    GLuint lightPositionUniform_Red;

    GLuint LaUniform_Blue;
    GLuint LdUniform_Blue;
    GLuint LsUniform_Blue;
    GLuint lightPositionUniform_Blue;

    GLuint KaUniform;
    GLuint KdUniform;
    GLuint KsUniform;
    GLuint KshineUniform;
    GLuint LKeyPressedUniform;

    mat4 gPerspectiveProjectionMatrix;

    GLfloat pyramidAngle = 0.0f;

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
       


 //vertex shader
    // create shader
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //Feed Shader
    const GLchar *vertexShaderSourceCode =
            "#version 440 core" \
            "\n" \
            "in vec4 vPosition;" \
            "in vec3 vNormal;" \
            "uniform mat4 u_model_matrix;" \
            "uniform mat4 u_view_matrix;" \
            "uniform mat4 u_perspective_projection_matrix;" \
            "uniform vec3 u_la_red;" \
            "uniform vec3 u_ld_red;" \
            "uniform vec3 u_ls_red;" \
            "uniform vec4 u_light_position_red;" \
            "uniform vec3 u_la_blue;" \
            "uniform vec3 u_ld_blue;" \
            "uniform vec3 u_ls_blue;" \
            "uniform vec4 u_light_position_blue;" \
            "uniform vec3 u_ka;" \
            "uniform vec3 u_kd;" \
            "uniform vec3 u_ks;" \
            "uniform int u_LKeyPressed;" \
            "uniform float u_shininess;" \
            "out vec3 phong_ads_light;" \
            "void main(void)" \
            "{" \
            "if(u_LKeyPressed == 1)" \
            "{" \
            "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
            "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
            "vec3 view_vector = normalize(-eye_coordinates.xyz);" \
            "vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));" \
            "vec3 reflection_vector_red = reflect(-light_direction_red,transformed_normal);" \
            "vec3 ambient_red = u_la_red * u_ka;" \
            "vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red,transformed_normal),0.0f);" \
            "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,view_vector),0.0f),u_shininess);" \
            "vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" \
            "vec3 reflection_vector_blue = reflect(-light_direction_blue,transformed_normal);" \
            "vec3 ambient_blue = u_la_blue * u_ka;" \
            "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue,transformed_normal),0.0f);" \
            "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,view_vector),0.0f),u_shininess);" \
            "phong_ads_light = ambient_red + ambient_blue + diffuse_red + diffuse_blue + specular_red + specular_blue;" \
            "}" \
            "else" \
            "{" \
            "phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
            "}" \
            "gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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

    // Fragment shader
    // crete shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
     //Feed Shader
    const GLchar *fragmentShaderSourceCode =
        "#version 440 core" \
        "\n" \
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(phong_ads_light,1.0f);" \
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
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

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
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // get MVP Uniform location
    modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
    viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
    perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
    LaUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_la_red");
    LdUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_ld_red");
    LsUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_ls_red");
    lightPositionUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_light_position_red");
    LaUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_la_blue");
    LdUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_ld_blue");
    LsUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_ls_blue");
    lightPositionUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_light_position_blue");
    KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    KshineUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");
    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject,"u_LKeyPressed");

    const GLfloat pyramidVertices[] = 
    {
        +0.0f,  +0.5f,  +0.0f,
        -0.5f,  -0.5f,  +0.5f,
        +0.5f,  -0.5f,  +0.5f,

        +0.0f,  +0.5f,  +0.0f,
        +0.5f,  -0.5f,  +0.5f,
        +0.5f,  -0.5f,  -0.5f,

        +0.0f,  +0.5f,  +0.0f,
        +0.5f,  -0.5f,  -0.5f,
        -0.5f,  -0.5f,  -0.5f,

        +0.0f,  +0.5f,  +0.0f,
        -0.5f,  -0.5f,  -0.5f,
        -0.5f,  -0.5f,  +0.5f
    };

    const GLfloat pyramidNormals[]  =
    {
        //FRONT FACE
        0.0f,   0.447214f,  0.894427f,
        0.0f,   0.447214f,  0.894427f,
        0.0f,   0.447214f,  0.894427f,
        0.0f,   0.447214f,  0.894427f,
        
        //RIGHT FACE
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        
        //BACK FACE
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        
        //LEFT FACE
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f

    };

    glGenVertexArrays(1, &gVao_Pyramid);
    glBindVertexArray(gVao_Pyramid);

        glGenBuffers(1, &gVbo_Position_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // position
        glGenBuffers(1, &gVbo_Normal_Pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
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

    bLight = false;

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

        case 'L':
        case 'l':
            if(bLight == true)
            {
                bLight = false;
            }
            else
            {
                bLight = true;
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

    // start using opengl program object
    glUseProgram(gShaderProgramObject);

    if(bLight == true)
    {
        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);

        glUniform4fv(lightPositionUniform_Red, 1, lightPosition_Red);
        glUniform3fv(LaUniform_Red, 1, lightAmbient_Red);
        glUniform3fv(LdUniform_Red, 1, lightDiffuse_Red);
        glUniform3fv(LsUniform_Red, 1, lightSpecular_Red);

        glUniform4fv(lightPositionUniform_Blue, 1, lightPosition_Blue);
        glUniform3fv(LaUniform_Blue, 1, lightAmbient_Blue);
        glUniform3fv(LdUniform_Blue, 1, lightDiffuse_Blue);
        glUniform3fv(LsUniform_Blue, 1, lightSpecular_Blue);

        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffuse);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    // opengl drawing
    // set modelview & modelviewprojection matrix to identity
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();
    mat4 TranslateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    mat4 RotationMatrix = vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);
    
    modelMatrix = TranslateMatrix * RotationMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, projectionMatrix);

    // bind vao
    glBindVertexArray(gVao_Pyramid);
        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        glDrawArrays(GL_TRIANGLES, 0, 12); // 12 total vertex
    // unbind vao
    glBindVertexArray(0);

    // stop using opengl program object
    glUseProgram(0);

    pyramidAngle += 0.1f;
    if (pyramidAngle >= 360.0f)
        pyramidAngle = 0.0f;


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