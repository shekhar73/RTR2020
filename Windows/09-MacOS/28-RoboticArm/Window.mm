#import <Foundation/Foundation.h>   //This is like stdio.h
#import <Cocoa/Cocoa.h> // anyonimous to windows.h
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>

#include "vmath.h"
#include "Sphere.h"

//using namespace vmath;
#define SIZE 32


vmath::mat4 MATRIXSTACK[SIZE];

int TOP = -1;
int shoulder;
int elbow;


void pushMatrix(vmath::mat4);
vmath::mat4 popMatrix(void);

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

        
    float sphere_vertices[1146];
    float shpere_normals[1146];
    float shpere_textures[746];
    unsigned short sphere_elements[2280];
    int gNumSphereVertices;
    int gNumSphereElements;

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_normal;
    GLuint vbo_element;

    GLuint modelMatrixUniform;
    GLuint perspectiveProjectionUniform;
    GLuint viewMatrixUniform;

    GLuint LaUniform;   // Light Ambient
    GLuint LsUniform;   // Light Specular
    GLuint LdUniform;   // Light Diffuse
    GLuint LpUniform;   // Light Position

    GLuint KaUniform;   // Material Ambient
    GLuint KdUniform;   // Diffuse
    GLuint KsUniform;   // Specular
    GLfloat MaterialShineUniform;

    GLuint lKeyPressedUniform;

    bool bAnimate;
    bool bLight;

    GLuint gVao_sphere;
    vmath::mat4 gPerspectiveProjectionMatrix;
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
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;"  \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;"    \
        "uniform mat4 u_projectionMatrix;"  \
        "uniform int u_lKeyPressed;"    \
        "uniform vec4 light_position;"  \
        "out vec3 transformed_Normal;"  \
        "out vec3 light_direction;" \
        "out vec3 view_vector;" \
        "void main(void)" \
        "\n" \
        "{" \
        "if(u_lKeyPressed == 1)"    \
        "{" \
        "vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;"    \
        "transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;"    \
        "light_direction = vec3(light_position - eyeCoordinate);"   \
        "view_vector = -eyeCoordinate.xyz;" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
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
        "#version 410 core" \
        "\n" \
        "vec3 phong_ads_light;" \
        "in vec3 transformed_Normal;"   \
        "in vec3 light_direction;"  \
        "in vec3 view_vector;"  \
        "out vec4 FragColor;"   \
        "uniform vec3 u_ld;"    \
        "uniform vec3 u_la;"    \
        "uniform vec3 u_ls;"    \
        "uniform vec4 light_position;"  \
        "uniform vec3 u_kd;"    \
        "uniform vec3 u_ka;"    \
        "uniform vec3 u_ks;"    \
        "uniform float materialShineUniform;"   \
        "uniform int u_lKeyPressed;"    \
        "void main(void)" \
        "\n" \
        "{" \
        "if(u_lKeyPressed == 1)"    \
        "{" \
            "vec3 normalizedTransformNormal = normalize(transformed_Normal);"   \
            "vec3 normalizedLightDirection = normalize(light_direction);"   \
            "vec3 normalizedViewVector = normalize(view_vector);"   \
            "vec3 ambient = u_la * u_ka;"   \
            "vec3 diffuse = u_ld * u_kd * max(dot(normalizedLightDirection, normalizedTransformNormal), 0.0f);" \
            "vec3 reflection_vector = reflect(-normalizedLightDirection, normalizedTransformNormal);"   \
            "vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalizedViewVector), 0.0f), materialShineUniform);" \
            "phong_ads_light = ambient + diffuse + specular;"   \
        "}" \
        "else"  \
        "{" \
            "phong_ads_light = vec3(1.0, 1.0, 1.0);"    \
        "}" \
        "FragColor = vec4(phong_ads_light, 1.0);" \
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

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

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
    modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
    viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");

    perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");

    lKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_lKeyPressed");
    LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    LpUniform = glGetUniformLocation(gShaderProgramObject, "light_position");

    KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    MaterialShineUniform = glGetUniformLocation(gShaderProgramObject, "materialShineUniform");

    getSphereVertexData(sphere_vertices, shpere_normals, shpere_textures, sphere_elements);
    gNumSphereVertices = getNumberOfSphereVertices();
    gNumSphereElements = getNumberOfSphereElements();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position
    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Normal
    glGenBuffers(1, &vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shpere_normals), shpere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Element drawing
    glGenBuffers(1, &vbo_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind vbo
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

    gPerspectiveProjectionMatrix = vmath::mat4::identity();

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
            
        case 'f':
        case 'F':
            [[self window]toggleFullScreen:self];
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

            case 'A':
            case 'a':
                if (bAnimate == true)
                {
                    bAnimate = false;
                }
                else
                {
                    bAnimate = true;
                }
                break;


            case 'S':
            case 's':
                shoulder = shoulder + 0.5f;
                break;

            case 'E':           
            case 'e':
                elbow = elbow + 0.5f;
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

    GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 120.0f, 120.0f, 100.0f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    GLfloat materialAmbient[] = { 1.5f, 1.5f, 0.5f, 1.0f };
    GLfloat materialDiffuse[] = { 1.7f, 1.7f, 0.7f, 1.0f };
    GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
    GLfloat materialShine = 128.0f;

    if (bLight == true)
    {
        glUniform1i(lKeyPressedUniform, 1);

        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LsUniform, 1, lightSpecular);
        glUniform4fv(LpUniform, 1, lightPosition);

        glUniform3fv(KdUniform, 1, materialDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KsUniform, 1, materialSpecular);
        glUniform1f(MaterialShineUniform, materialShine);
    }
    else
    {
        glUniform1i(lKeyPressedUniform, 0);
    }

    // OpenGL Drawing
    // set modelview & modelviewprojection matrices to identity
    vmath::mat4 translateMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrixX = vmath::mat4::identity();
    vmath::mat4 rotationMatrixY = vmath::mat4::identity();
    vmath::mat4 rotationMatrixZ = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    translateMatrix = vmath::translate(0.0f, 0.0f, -12.0f);
    modelMatrix = translateMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    pushMatrix(modelMatrix);
        rotationMatrixZ = vmath::rotate((GLfloat)shoulder, 0.0f, 0.0f, 1.0f);
        translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
        modelMatrix = modelMatrix * rotationMatrixZ * translateMatrix;

        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        pushMatrix(modelMatrix);
            scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
            modelMatrix = modelMatrix * scaleMatrix;
            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

            glBindVertexArray(vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
                glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);
    modelMatrix = popMatrix();



    translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
    rotationMatrixZ = vmath::rotate((GLfloat)elbow, 0.0f, 0.0f, 1.0f);
    modelMatrix = modelMatrix * translateMatrix * rotationMatrixZ;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
    modelMatrix = modelMatrix * translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    pushMatrix(modelMatrix);
    scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
    modelMatrix = modelMatrix * scaleMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    modelMatrix = popMatrix();

    modelMatrix = popMatrix();

    // Stop using OpenGL program object
    glUseProgram(0);


    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}


void pushMatrix(vmath::mat4 matrix)
{
    if (TOP < SIZE - 1)
    {
        TOP++;
        MATRIXSTACK[TOP] = matrix;
    }

}

vmath::mat4 popMatrix()
{
    vmath::mat4 ret;
    if (TOP > 0)
    {
        int temp = TOP;
        ret = MATRIXSTACK[temp];
        TOP--;
    }
    return(ret);
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
