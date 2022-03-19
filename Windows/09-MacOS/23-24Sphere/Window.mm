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


GLfloat factor = 10.0f;
GLfloat angle_X = 0.0f;
GLfloat angle_Y = 0.0f;
GLfloat angle_Z = 0.0f;


GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };


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

    int keyPressed_1;
    int keyPressed_2;
    int keyPressed_3;


    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLfloat sphere_vertices[1146];
    GLfloat sphere_normals[1146];
    GLfloat sphere_textures[764];
    unsigned short sphere_elements[2280];

    GLuint gVao_sphere;
    GLuint gNumVertices;
    GLuint gNumElements;

    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_element;

    GLfloat materialAmbient[4];
    GLfloat materialDiffused[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;

    GLuint gModelMatrixUniform;
    GLuint gViewMatrixUniform;
    GLuint gPerspectiveProjectionMatrixUniform;

    // Light
    GLuint LaUniform;
    GLuint LdUniform;
    GLuint LsUniform;
    GLuint lightPositionUniform;

    // Material
    GLuint KaUniform;
    GLuint KdUniform;
    GLuint KsUniform;
    GLfloat KshineUniform;

    GLuint LKeyPressedUniform;

    bool bLight;
    
    // It is a matrix of 4 X 4 which is declared in vmath.h header file. It will be used for transformation
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
    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_perspective_projection_matrix;" \
        "uniform int u_l_key_preesed;" \
        "uniform vec4 u_light_position;" \
        "out vec3 light_direction;" \
        "out vec3 view_vector;" \
        "out vec3 transformed_normal;" \
        "void main(void)" \
        "{" \
            "if(u_l_key_preesed == 1)" \
            "{" \
                "vec4 eye_coordinates       = u_view_matrix * u_model_matrix * vPosition;" \
                "transformed_normal         = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
                "view_vector                = -eye_coordinates.xyz;" \
                "light_direction            = vec3(u_light_position - eye_coordinates);" \
            "}" \
            "gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix  * vPosition;"  \
        "}";

    // eye_coordinates.xyz = swizling
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
    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "out vec4 FragColor;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform int u_l_key_preesed;" \
        "in vec3 light_direction;" \
        "in vec3 view_vector;" \
        "in vec3 transformed_normal;" \
        "uniform float u_shineness;" \
        "void main(void)" \
        "{" \
            "vec3 phong_ads_light;" \
            "if(u_l_key_preesed == 1)" \
            "{" \
                "vec3 normalized_transformed_normals    = normalize(transformed_normal);" \
                "vec3 normalized_light_directions       = normalize(light_direction);" \
                "vec3 normalized_view_vectors           = normalize(view_vector);" \
                "vec3 reflection_vector                 = reflect(-normalized_light_directions, normalized_transformed_normals);" \
                "vec3 ambient_light                     = u_la * u_ka;" \
                "vec3 diffuse_light                     = u_ld * u_kd * max(dot(normalized_light_directions, normalized_transformed_normals), 0.0f);" \
                "vec3 specular                          = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vectors), 0.0f), u_shineness);" \
                "phong_ads_light                        = ambient_light + diffuse_light + specular;" \
            "}" \
            "else"  \
            "{" \
                "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
            "}" \
            "FragColor = vec4(phong_ads_light, 1.0f);" \
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


    gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
    gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
    gPerspectiveProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
    LaUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    LdUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    LsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    KaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    KdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    KsUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    KshineUniform = glGetUniformLocation(gShaderProgramObject, "u_shineness");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_l_key_preesed");
    


    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

    // It will store all the below 6 steps so that we can reuse it in draw function
    glGenVertexArrays(1, &gVao_sphere);
    glBindVertexArray(gVao_sphere);

    // Postion vbo
    glGenBuffers(1, &gVbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Normal vbo
    glGenBuffers(1, &gVbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Element vbo
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Unbind Vao
    glBindVertexArray(0);

    //glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    // Unbind Vao
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
    bLight = false;
    
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

        case 'X':
        case 'x':
            keyPressed_1 = 1;
            angle_X = 0.0f;
            break;

        case 'Y':
        case 'y':
            keyPressed_2 = 2;
            angle_Y = 0.0f;
            break;

        case 'Z':
        case 'z':
            keyPressed_3 = 3;
            angle_Z = 0.0f;
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

   // || 1 Sphere|| Emerald ----------------------------------------------------------------------
    // start using OpenGL program object
    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0215;
    materialAmbient[1] = 0.1745;
    materialAmbient[2] = 0.0215;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.07568;
    materialDiffused[1] = 0.61424;
    materialDiffused[2] = 0.07568;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.633;
    materialSpecular[1] = 0.727811;
    materialSpecular[2] = 0.633;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.6f * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();
    mat4 translateMatrix = vmath::translate(1.5f, 14.0f, -20.0f);
    mat4 scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;
    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // Jade || 2 Sphere||----------------------------------------------------------------------
    
    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.135f;
    materialAmbient[1] = 0.2225f;
    materialAmbient[2] = 0.1575f;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.54f;
    materialDiffused[1] = 0.89f;
    materialDiffused[2] = 0.63f;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.316228f;
    materialSpecular[1] = 0.316228f;
    materialSpecular[2] = 0.316228f;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.1f * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(26.0f, 14.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // || obsidian 3 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.05375;
    materialAmbient[1] = 0.05;
    materialAmbient[2] = 0.06625;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.18275;
    materialDiffused[1] = 0.17;
    materialDiffused[2] = 0.22525;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.332741;
    materialSpecular[1] = 0.328634;
    materialSpecular[2] = 0.346435;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.3 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(11.5f, 14.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // || pearl 4 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.25;
    materialAmbient[1] = 0.20725;
    materialAmbient[2] = 0.20725;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 1.0;
    materialDiffused[1] = 0.829;
    materialDiffused[2] = 0.829;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.296648;
    materialSpecular[1] = 0.296648;
    materialSpecular[2] = 0.296648;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.088 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(6.5f, 14.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // || ruby 5 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.1745;
    materialAmbient[1] = 0.01175;
    materialAmbient[2] = 0.01175;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.61424;
    materialDiffused[1] = 0.04136;
    materialDiffused[2] = 0.04136;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.727811;
    materialSpecular[1] = 0.626959;
    materialSpecular[2] = 0.626959;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.6 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(16.5f, 14.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    

    // || turquoise 6 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.1;
    materialAmbient[1] = 0.18725;
    materialAmbient[2] = 0.1745;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.396;
    materialDiffused[1] = 0.74151;
    materialDiffused[2] = 0.69102;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.297254;
    materialSpecular[1] = 0.30829;
    materialSpecular[2] = 0.306678;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.1 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(21.5f, 14.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // ||brass 7 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.329412;
    materialAmbient[1] = 0.223529;
    materialAmbient[2] = 0.027451;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.780392;
    materialDiffused[1] = 0.568627;
    materialDiffused[2] = 0.113725;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.992157;
    materialSpecular[1] = 0.941176;
    materialSpecular[2] = 0.807843;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.21794872 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(1.5f, 10.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // || bronze 8 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.2125;
    materialAmbient[1] = 0.1275;
    materialAmbient[2] = 0.054;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.714;
    materialDiffused[1] = 0.4284;
    materialDiffused[2] = 0.18144;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.393548;
    materialSpecular[1] = 0.271906;
    materialSpecular[2] = 0.166721;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.2 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(21.5f, 14.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || chrome 9 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.25;
    materialAmbient[1] = 0.25;
    materialAmbient[2] = 0.25;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.4;
    materialDiffused[1] = 0.4;
    materialDiffused[2] = 0.4;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.774597;
    materialSpecular[1] = 0.774597;
    materialSpecular[2] = 0.774597;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.6 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(6.5f, 10.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || copper 10 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.19125;
    materialAmbient[1] = 0.0735;
    materialAmbient[2] = 0.0225;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.7038;
    materialDiffused[1] = 0.27048;
    materialDiffused[2] = 0.0828;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.256777;
    materialSpecular[1] = 0.137622;
    materialSpecular[2] = 0.086014;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.1 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(11.5f, 10.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || gold 11 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.24725;
    materialAmbient[1] = 0.1995;
    materialAmbient[2] = 0.0745;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.75164;
    materialDiffused[1] = 0.60648;
    materialDiffused[2] = 0.22648;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.628281;
    materialSpecular[1] = 0.555802;
    materialSpecular[2] = 0.366065;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.4 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(16.5f, 10.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // ||silver 12 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.19225;
    materialAmbient[1] = 0.19225;
    materialAmbient[2] = 0.19225;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.50754;
    materialDiffused[1] = 0.50754;
    materialDiffused[2] = 0.50754;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.508273;
    materialSpecular[1] = 0.508273;
    materialSpecular[2] = 0.508273;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.4 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(21.5f, 10.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // 3rd Coloumn
    //////////////////////////////////////////////////////////
    // black  13
    //////////////////////////////////////////////////////////

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.0;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.01;
    materialDiffused[1] = 0.01;
    materialDiffused[2] = 0.01;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.50;
    materialSpecular[1] = 0.50;
    materialSpecular[2] = 0.50;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.25 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(26.0f, 10.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || cyan 14 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.1;
    materialAmbient[2] = 0.06;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.0;
    materialDiffused[1] = 0.50980392;
    materialDiffused[2] = 0.50980392;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.50196078;
    materialSpecular[1] = 0.50196078;
    materialSpecular[2] = 0.50196078;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.25 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(1.5f, 6.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || green 15 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.0;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.1;
    materialDiffused[1] = 0.35;
    materialDiffused[2] = 0.1;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.45;
    materialSpecular[1] = 0.55;
    materialSpecular[2] = 0.45;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.25 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(6.5f, 6.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || red 16 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.0;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.5;
    materialDiffused[1] = 0.0;
    materialDiffused[2] = 0.0;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.7;
    materialSpecular[1] = 0.6;
    materialSpecular[2] = 0.6;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.25 * 128;


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(11.5f, 6.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || white 17 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.0;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.55;
    materialDiffused[1] = 0.55;
    materialDiffused[2] = 0.55;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.70;
    materialSpecular[1] = 0.70;
    materialSpecular[2] = 0.70;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.25 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(16.5f, 6.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || yellow plastic 18 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.0;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.5;
    materialDiffused[1] = 0.5;
    materialDiffused[2] = 0.0;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.60;
    materialSpecular[1] = 0.60;
    materialSpecular[2] = 0.50;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.25 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(21.5f, 6.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    /// 4th coloumn
    //////////////////////////////////////////////////////////
    // black 19
    //////////////////////////////////////////////////////////

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.02;
    materialAmbient[1] = 0.02;
    materialAmbient[2] = 0.02;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.01;
    materialDiffused[1] = 0.01;
    materialDiffused[2] = 0.01;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.4;
    materialSpecular[1] = 0.4;
    materialSpecular[2] = 0.4;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(26.0f, 6.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);
    

    // || cyan 20 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.05;
    materialAmbient[2] = 0.05;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.4;
    materialDiffused[1] = 0.5;
    materialDiffused[2] = 0.5;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.04;
    materialSpecular[1] = 0.7;
    materialSpecular[2] = 0.7;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(1.5f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);
    

    // || green 21 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    materialAmbient[0] = 0.0;
    materialAmbient[1] = 0.05;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.4;
    materialDiffused[1] = 0.5;
    materialDiffused[2] = 0.4;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.04;
    materialSpecular[1] = 0.7;
    materialSpecular[2] = 0.04;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(6.5f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || red 22 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.05;
    materialAmbient[1] = 0.0;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.5;
    materialDiffused[1] = 0.4;
    materialDiffused[2] = 0.4;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.7;
    materialSpecular[1] = 0.04;
    materialSpecular[2] = 0.04;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(11.5f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);


    // || white 23 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.05;
    materialAmbient[1] = 0.05;
    materialAmbient[2] = 0.05;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.5;
    materialDiffused[1] = 0.5;
    materialDiffused[2] = 0.5;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.7;
    materialSpecular[1] = 0.7;
    materialSpecular[2] = 0.7;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(16.5f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // ||  yellow rubber  24 Sphere||----------------------------------------------------------------------

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.05;
    materialAmbient[1] = 0.05;
    materialAmbient[2] = 0.0;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.5;
    materialDiffused[1] = 0.5;
    materialDiffused[2] = 0.4;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.7;
    materialSpecular[1] = 0.7;
    materialSpecular[2] = 0.04;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(21.5f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // 25
    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.15;
    materialAmbient[1] = 0.15;
    materialAmbient[2] = 0.01;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.5;
    materialDiffused[1] = 0.25;
    materialDiffused[2] = 0.14;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.27;
    materialSpecular[1] = 0.37;
    materialSpecular[2] = 0.04;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(26.0f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    // 26
    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        if (keyPressed_1 == 1)
        {
            angle_X = angle_X + 0.001f;
            lightPosition[0] = 0.0f;
            lightPosition[1] = factor * sin(angle_X);
            lightPosition[2] = factor * cos(angle_X);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_2 == 2)
        {
            angle_Y = angle_Y + 0.001f;
            lightPosition[0] = factor * sin(angle_Y);
            lightPosition[1] = 0.0f;
            lightPosition[2] = factor * cos(angle_Y);
            lightPosition[3] = 1.0f;
        }
        else if (keyPressed_3 == 3)
        {
            angle_Z = angle_Z + 0.001f;
            lightPosition[0] = factor * sin(angle_Z);
            lightPosition[1] = factor * cos(angle_Z);
            lightPosition[2] = 0.0f;
            lightPosition[3] = 1.0f;
        }

        glUniform1i(LKeyPressedUniform, 1);
        glUniform1f(KshineUniform, materialShininess);
        glUniform4fv(lightPositionUniform, 1, lightPosition);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffused);
        glUniform3fv(KsUniform, 1, materialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    glBindVertexArray(gVao_sphere);

    materialAmbient[0] = 0.25;
    materialAmbient[1] = 0.35;
    materialAmbient[2] = 0.41;
    materialAmbient[3] = 1.0f;

    materialDiffused[0] = 0.25;
    materialDiffused[1] = 0.35;
    materialDiffused[2] = 0.44;
    materialDiffused[3] = 1.0f;

    materialSpecular[0] = 0.57;
    materialSpecular[1] = 0.47;
    materialSpecular[2] = 0.34;
    materialSpecular[3] = 1.0f;

    materialShininess = 0.078125 * 128;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    translateMatrix = vmath::translate(1.5f, 2.0f, 0.0f);
    scaleMatrix = vmath::scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

    if (angle_X >= 360.0f)
    {
        angle_X = 0.0f;
    }

    if (angle_Y >= 360.0f)
    {
        angle_Y = 0.0f;
    }

    if (angle_Z >= 360.0f)
    {
        angle_Z = 0.0f;
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
