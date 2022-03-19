#import <Foundation/Foundation.h>   //This is like stdio.h
#import <Cocoa/Cocoa.h> // anyonimous to windows.h
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>

#include "vmath.h"
#include "Sphere.h"

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

    GLuint gVao_Sphere;
    GLuint gVbo_Sphere_Position;
    GLuint gVbo_Sphere_Normals;
    GLuint gVbo_Sphere_Elements;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLfloat sphere_vertices[1146];
    GLfloat sphere_normals[1146];
    GLfloat sphere_texture[764];
    unsigned short sphere_elements[2280];
    
    GLuint ldUniform;
    GLuint kdUniform;
    GLuint lightPositionUniform;
    GLuint LKeyPressedUniform;
    
    GLuint gNumVertices;
    GLuint gNumElements;

    int day;
    int year;
    int moon;

    GLuint bLight;
    GLuint bAnimate;
    GLfloat angleCube;
    
    // It is a matrix of 4 X 4 which is declared in vmath.h header file. It will be used for transformation
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
       


    ///////////////////////////////////////////////////////////////////////////////////////
    //*** VERTEX SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create Shader
   gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //provide source code to shader
    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform int u_LKeyPressed;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec4 u_Light_Position;" \
        "out vec3 diffuse_light;" \
        "void main(void)" \
        "\n" \
        "{" \
            "if(u_LKeyPressed == 1)" \
            "{" \
                    "vec4 eye_coordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
                    "mat3 normal_matrix = mat3(transpose(inverse(u_viewMatrix * u_modelMatrix)));" \
                    "vec3 tNormal = normalize(normal_matrix * vNormal);" \
                    "vec3 S = normalize(vec3(u_Light_Position - eye_coordinate));" \
                    "diffuse_light = u_ld * u_kd * max(dot(S,tNormal),0.0);" \
            "}" \
                "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
        "}";
    
    // eye_coordinates.xyz = swizling
    //gl_Position is inbuilt variable in shader
    // feed the source code to your shaderobject
    glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    //compile shader
    glCompileShader(gVertexShaderObject);

    GLint iInfoLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;
    
    // shader compilation error checking

    //void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // glGetShaderiv — return a parameter from a shader object

    glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szInfoLog = (char*)malloc(sizeof(char) * iInfoLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
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

    // provide Source Code to Shader
    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "vec4 color;" \
        "in vec3 diffuse_light;" \
        "uniform int u_LKeyPressed;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "\n" \
        "{" \
                "if(u_LKeyPressed == 1)" \
                "{" \
                    "color = vec4(diffuse_light,1.0);" \
                "}" \
                "else" \
                "{" \
                    "color = vec4(1.0,1.0,1.0,1.0);" \
                "}" \
                "FragColor = color;" \
        "}";

    //Here we fill the data in the Shader object
    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    //compile shader
    glCompileShader(gFragmentShaderObject);

    iInfoLength = 0;
    iShaderCompiledStatus = 0;

    // fragment shader compilation error checking

    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        //we use this call to get the length of the info
        glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szInfoLog = (char*) malloc(sizeof(char) * iInfoLength);
            //Error checking if malloc is allocated memory
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
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


    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

    //link shader
    glLinkProgram(gShaderProgramObject);

    // Reset values
    GLint iShaderProgramLinkStatus = 0;
    iInfoLength = 0;
    
    // shader link error checking
    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szInfoLog = (char*)malloc(sizeof(char) * iInfoLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // get MVP uniform location
    // post linking

    modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
    viewMatrixUniform  = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
    projectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");
    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_Light_Position");

  
    getSphereVertexData(sphere_vertices, sphere_normals, sphere_texture, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

    // It will store all the below 6 steps so that we can reuse it in draw function
    glGenVertexArrays(1, &gVao_Sphere);
    glBindVertexArray(gVao_Sphere);

    // Postion vbo
    glGenBuffers(1, &gVbo_Sphere_Position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Sphere_Position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Normal vbo
    glGenBuffers(1, &gVbo_Sphere_Normals);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Sphere_Normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Element vbo
    glGenBuffers(1, &gVbo_Sphere_Elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_Sphere_Elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Unbind Vao
    glBindVertexArray(0);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
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
    gPerspectiveProjectionMatrix = perspective(45.0f, ((GLfloat)rect.size.width / (GLfloat)rect.size.height), 0.1f, 100.0f);
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
            
  
        case 'D':
            day = (day + 6) % 360;
            break;

        case 'd':
            day = (day - 6) % 360;
            break;

        case 'Y':
            year = (year + 3) % 360;
            break;

        case 'y':
            year = (year - 3) % 360;
            break;

        case 'M':
            moon = (moon + 3) % 360;
            break;

        case 'm':
            moon = (moon - 3) % 360;
            break;
    }
}


-(void)drawView
{
    //code
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gShaderProgramObject);

    
    if(bLight == true)
    {
        //Light
        glUniform1i(LKeyPressedUniform, 1);
        glUniform3f(ldUniform, 1.0, 1.0, 1.0);
        glUniform3f(kdUniform, 0.50, 0.50, 0.50);

        GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f };
        glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

    }
    else
    {
        glUniform1i(LKeyPressedUniform,0);
    }
    
    // OpenGL Drawing - set modelview & modelviewprojection matrices to identity
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 translateMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix  = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f,0.0f,-5.0f);
    
    scaleMatrix = scale(1.75f, 1.75f, 1.75f);
    rotationMatrix  = rotate(0.0f,1.0f,0.0f,0.0f);
    modelMatrix = translateMatrix * scaleMatrix * rotationMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform,1,GL_FALSE,modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform,1,GL_FALSE,viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform,1,GL_FALSE,projectionMatrix);
    
    glBindVertexArray(gVao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    
 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    rotationMatrix  = modelMatrix * rotate((GLfloat)year,0.0f,1.0f,0.0f);
    translateMatrix = translate(1.0f,0.0f,-5.0f);
    mat4 rotationMatrix_1  = modelMatrix * rotationMatrix * rotate((GLfloat)day,0.0f,1.0f,0.0f);
    scaleMatrix = scale(0.90f, 0.90f, 0.90f);
    
    modelMatrix =  rotationMatrix * translateMatrix * rotationMatrix_1 * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;
    
    glUniformMatrix4fv(modelMatrixUniform,1,GL_FALSE,modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform,1,GL_FALSE,viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform,1,GL_FALSE,projectionMatrix);
    
    glBindVertexArray(gVao_Sphere);    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);

    rotationMatrix  = modelMatrix * rotate((GLfloat)moon,0.0f,1.0f,0.0f);
    translateMatrix = translate(0.8f,0.0f,-1.0f);
    scaleMatrix = scale(0.3f, 0.3f, 0.3f);    
    modelMatrix = rotationMatrix * translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;
    
    glUniformMatrix4fv(modelMatrixUniform,1,GL_FALSE,modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform,1,GL_FALSE,viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform,1,GL_FALSE,projectionMatrix);
    

    glBindVertexArray(gVao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);

    glUseProgram(0);

    if(bAnimate == true)
    {
        angleCube = angleCube + 0.50f;
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
