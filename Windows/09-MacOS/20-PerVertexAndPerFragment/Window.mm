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

    
    // Per Vertex Shader
    GLuint gVertexShaderObject_PV;
    GLuint gFragmentShaderObject_PV;
    GLuint gShaderProgramObject_PV;

    //////////////////////////////////////
    // PER VERTEX UNIFORMS
    ///////////////////////////////////////
    GLuint gModelMatrix_PV;
    GLuint gViewMatrix_PV;
    GLuint gProjectionMatrix_PV;

    // Per Vertex Light
    GLuint LaUniform_PV;
    GLuint LdUniform_PV;
    GLuint LsUniform_PV;

    // Per Vertex Material
    GLuint KaUniform_PV;
    GLuint KdUniform_PV;
    GLuint KsUniform_PV;

    GLfloat materialShineness_PV;
    GLuint lightPositionUniform_PV;


    // Per Fragment Shader
    GLuint gVertexShaderObject_PF;
    GLuint gFragmentShaderObject_PF;
    GLuint gShaderProgramObject_PF;

    //////////////////////////////////////
    // PER FRAGMENT UNIFORMS
    ///////////////////////////////////////
    GLuint gModelMatrix_PF;
    GLuint gViewMatrix_PF;
    GLuint gProjectionMatrix_PF;

    // Per Vertex Light
    GLuint LaUniform_PF;
    GLuint LdUniform_PF;
    GLuint LsUniform_PF;

    // Per Vertex Material
    GLuint KaUniform_PF;
    GLuint KdUniform_PF;
    GLuint KsUniform_PF;

    GLfloat materialShineness_PF;
    GLuint lightPositionUniform_PF;

    GLfloat sphere_vertices[1146];
    GLfloat sphere_normals[1146];
    GLfloat sphere_textures[764];
    unsigned short sphere_elements[2280];

    GLuint gVao_sphere;
    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_element;

    GLuint chooseShader = 1;
    GLuint gNumVertices;
    GLuint gNumElements;

    GLuint LKeyPressedUniform;
    bool bLight;

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
    //***PER VERTEX SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create Shader
    gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

    // Provide source code to shader
    const GLchar* vertexShaderSourceCode_PV =
        "#version 450 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform int u_l_key_preesed;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ks;" \
        "uniform vec4 u_light_position;" \
        "uniform float u_shineness;" \
        "out vec3 fong_ads_light;" \
        "void main(void)" \
        "{" \
        "if(u_l_key_preesed == 1)" \
        "{" \
        "vec4 eye_coordinates       = u_viewMatrix * u_modelMatrix * vPosition;" \
        "vec3 transformed_normal    = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
        "vec3 light_direction       = normalize(vec3(u_light_position - eye_coordinates));" \
        "vec3 reflection_vector     = reflect(-light_direction, transformed_normal);" \
        "vec3 view_vector           = normalize(-eye_coordinates.xyz);" \
        "vec3 ambient_light         = u_la * u_ka;" \
        "vec3 diffuse_light         = u_ld * u_kd * max(dot(light_direction, transformed_normal), 0.0f);" \
        "vec3 specular              = u_ls * u_ks * pow(max(dot(reflection_vector, view_vector), 0.0f), u_shineness);" \
        "fong_ads_light             = ambient_light + diffuse_light + specular;" \
        "}" \
        "else"  \
        "{" \
        "fong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix  * vPosition;"  \
        "}";

    // eye_coordinates.xyz = swizling
    //gl_Position is inbuilt variable in shader
    // feed the source code to your shaderobject
    glShaderSource(gVertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

    // compile shader
    glCompileShader(gVertexShaderObject_PV);

    GLint iInfoLogLength_PV = 0;
    GLint iShaderCompiledStatus_PV = 0;
    char* szInfoLog_PV = NULL;

    // shader compilation error checking

    //void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // glGetShaderiv — return a parameter from a shader object

    glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus_PV);
    if (iShaderCompiledStatus_PV == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength_PV);
        if (iInfoLogLength_PV > 0)
        {
            szInfoLog_PV = (char*)malloc(iInfoLogLength_PV);
            if (szInfoLog_PV != NULL)
            {
                GLsizei written_PV;

                // take log
                glGetShaderInfoLog(gVertexShaderObject_PV, iInfoLogLength_PV, &written_PV, szInfoLog_PV);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog_PV);
                free(szInfoLog_PV);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //*** PER VERTEX FRAGMENT SHADER ***
    ///////////////////////////////////////////////////////////////////////////////////////

    // Create shader
    gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
    const GLchar* fragmentShaderSourceCode_PV =
        "#version 450 core" \
        "\n" \
        "out vec4 FragColor;" \
        "in vec3 fong_ads_light;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(fong_ads_light, 1.0f);" \
        "}";


    glShaderSource(gFragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

    // compile shader
    glCompileShader(gFragmentShaderObject_PV);

    // fragment shader compilation error checking


    glGetShaderiv(gFragmentShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus_PV);
    if (iShaderCompiledStatus_PV == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength_PV);
        if (iInfoLogLength_PV > 0)
        {
            szInfoLog_PV = (char*)malloc(iInfoLogLength_PV);
            if (szInfoLog_PV != NULL)
            {
                GLsizei written_PV;
                glGetShaderInfoLog(gFragmentShaderObject_PV, iInfoLogLength_PV, &written_PV, szInfoLog_PV);
                fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog_PV);
                free(szInfoLog_PV);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //*** PER VERTEX SHADER PROGRAM ***
    /////////////////////////////////////////////////////////////////////////////////////

    // create
    gShaderProgramObject_PV = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
    glBindAttribLocation(gShaderProgramObject_PV, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject_PV, SSK_ATTRIBUTE_NORMAL, "vNormal");

    // link shader
    glLinkProgram(gShaderProgramObject_PV);

    GLint iShaderProgramLinkStatus_PV = 0;


    // shader link error checking
    glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderProgramLinkStatus_PV);

    if (iShaderProgramLinkStatus_PV == GL_FALSE)
    {
        glGetShaderiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength_PV);
        if (iInfoLogLength_PV > 0)
        {
            szInfoLog_PV = (char*)malloc(iInfoLogLength_PV);
            if (szInfoLog_PV != NULL)
            {
                GLsizei written_PV;
                glGetShaderInfoLog(gShaderProgramObject_PV, iInfoLogLength_PV, &written_PV, szInfoLog_PV);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog_PV);
                free(szInfoLog_PV);
                [self release];
                [NSApp terminate:self];
            }
        }

    }

    // get MVP uniform location
    // post linking


    gModelMatrix_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_modelMatrix");
    gViewMatrix_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_viewMatrix");
    gProjectionMatrix_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projectionMatrix");


    LaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_la");
    

    LdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ld");
   

    LsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ls");
   

    KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ka");
    

    KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_kd");
   

    KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ks");
   


    materialShineness_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_shineness");
   

    lightPositionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position");
   

    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_l_key_preesed");
   





    ///////////////////////////////////////////////////////////////////////////////////////
    //***PER FRAGMENT VERTEX SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////


    // Create Shader
    gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

    // Provide source code to shader
    const GLchar* vertexShaderSourceCode_PF =
        "#version 450 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform int u_l_key_preesed;" \
        "uniform vec4 u_light_position;" \
        "out vec3 light_direction;" \
        "out vec3 view_vector;" \
        "out vec3 transformed_normal;" \
        "void main(void)" \
        "{" \
        "if(u_l_key_preesed == 1)" \
        "{" \
        "vec4 eye_coordinates       = u_viewMatrix * u_modelMatrix * vPosition;" \
        "transformed_normal         = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
        "light_direction            = vec3(u_light_position - eye_coordinates);" \
        "view_vector                = -eye_coordinates.xyz;" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix  * vPosition;"  \
        "}";

    // eye_coordinates.xyz = swizling
    //gl_Position is inbuilt variable in shader
    // feed the source code to your shaderobject
    glShaderSource(gVertexShaderObject_PF, 1, (const GLchar**)&vertexShaderSourceCode_PF, NULL);

    // compile shader
    glCompileShader(gVertexShaderObject_PF);

    GLint iInfoLogLength_PF = 0;
    GLint iShaderCompiledStatus_PF = 0;
    char* szInfoLog_PF = NULL;

    // shader compilation error checking

    //void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // glGetShaderiv — return a parameter from a shader object

    glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus_PF);
    if (iShaderCompiledStatus_PF == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength_PF);
        if (iInfoLogLength_PF > 0)
        {
            szInfoLog_PF = (char*)malloc(iInfoLogLength_PF);
            if (szInfoLog_PF != NULL)
            {
                GLsizei written;

                // take log
                glGetShaderInfoLog(gVertexShaderObject_PF, iInfoLogLength_PF, &written, szInfoLog_PF);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog_PF);
                free(szInfoLog_PF);
               [self release];
                [NSApp terminate:self];
            }
        }
    }








    ///////////////////////////////////////////////////////////////////////////////////////
    //***PER FRAGMENT FRAGMENT SHADER ***
    //////////////////////////////////////////////////////////////////////////////////////

    // Create shader
    gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

    // provide source code to shader
    const GLchar* fragmentShaderSourceCode_PF =
        "#version 450 core" \
        "\n" \
        "out vec4 FragColor;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ks;" \
        "uniform int u_l_key_preesed;" \
        "in vec3 light_direction;" \
        "in vec3 view_vector;" \
        "in vec3 transformed_normal;" \
        "in vec3 reflection_vector;" \
        "uniform float u_shineness;" \
        "vec3 fong_ads_light;" \
        "void main(void)" \
        "{" \
        "if(u_l_key_preesed == 1)" \
        "{" \
        "vec3 normalized_transformed_normals    = normalize(transformed_normal);" \
        "vec3 normalized_light_directions       = normalize(light_direction);" \
        "vec3 normalized_view_vectors           = normalize(view_vector);" \
        "vec3 reflection_vector                 = reflect(-normalized_light_directions, normalized_transformed_normals);" \
        "vec3 ambient_light                     = u_la * u_ka;" \
        "vec3 diffuse_light                     = u_ld * u_kd * max(dot(normalized_light_directions, normalized_transformed_normals), 0.0f);" \
        "vec3 specular                          = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vectors), 0.0f), u_shineness);" \
        "fong_ads_light                         = ambient_light + diffuse_light + specular;" \
        "}" \
        "else"  \
        "{" \
        "fong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
        "}" \
        "FragColor = vec4(fong_ads_light, 1.0f);" \
        "}";


    glShaderSource(gFragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

    // compile shader
    glCompileShader(gFragmentShaderObject_PF);

    // fragment shader compilation error checking


    glGetShaderiv(gFragmentShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus_PF);
    if (iShaderCompiledStatus_PF == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength_PF);
        if (iInfoLogLength_PF > 0)
        {
            szInfoLog_PF = (char*)malloc(iInfoLogLength_PF);
            if (szInfoLog_PF != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject_PF, iInfoLogLength_PF, &written, szInfoLog_PF);
                fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog_PF);
                free(szInfoLog_PF);
               [self release];
                [NSApp terminate:self];
            }
        }
    }




    

    /////////////////////////////////////////////////////////////////////////////////////
    //*** PER FRAGMENT SHADER PROGRAM ***
    /////////////////////////////////////////////////////////////////////////////////////

    // create
    gShaderProgramObject_PF = glCreateProgram();

    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);

    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
    glBindAttribLocation(gShaderProgramObject_PF, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject_PF, SSK_ATTRIBUTE_NORMAL, "vNormal");

    // link shader
    glLinkProgram(gShaderProgramObject_PF);

    GLint iShaderProgramLinkStatus_PF = 0;

    // shader link error checking
    glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderProgramLinkStatus_PF);

    if (iShaderProgramLinkStatus_PF == GL_FALSE)
    {
        glGetShaderiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength_PF);
        if (iInfoLogLength_PF > 0)
        {
            szInfoLog_PF = (char*)malloc(iInfoLogLength_PF);
            if (szInfoLog_PF != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gShaderProgramObject_PF, iInfoLogLength_PF, &written, szInfoLog_PF);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog_PF);
                free(szInfoLog_PF);
                [self release];
                [NSApp terminate:self];
            }
        }

    }

    // get MVP uniform location
    // post linking


    gModelMatrix_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_modelMatrix");
   

    gViewMatrix_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_viewMatrix");
    

    gProjectionMatrix_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projectionMatrix");
    

    LaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_la");
    

    LdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ld");
    

    LsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ls");
    

    KaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ka");
    
    KdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_kd");
   

    KsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ks");
    


    materialShineness_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_shineness");
    

    lightPositionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position");
    

    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_l_key_preesed");
    





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


   // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    // we will always cull back faces for better performance

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

        case 49:
            chooseShader = 1;
            break;

        case 50:
            chooseShader = 2;
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

   // Light
    static GLfloat LaUniforms[] = { 0.1f, 0.1f, 0.1f };
    static GLfloat LdUniforms[] = { 1.0f, 1.0f, 1.0f };
    static GLfloat LsUniforms[] = { 1.0f, 1.0f, 1.0f };

    // Material
    static GLfloat KaUniforms[] = { 0.0f, 0.0f, 0.0f };
    static GLfloat KdUniforms[] = { 0.5f, 0.2f, 0.7f };
    static GLfloat KsUniforms[] = { 0.7f, 0.7f, 0.7f };
    static GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };  //if w(4th parameter) = 0.0f then it is directional light or if w = 1.0f then it is positional light

    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    mat4 ModelMatrix = mat4::identity();
    mat4 ViewMatrix = mat4::identity();
    mat4 ProjectionMatrix = mat4::identity();


    if (chooseShader == 1)
    {
        fprintf(gpFile, "%d\n", __LINE__);
        // start using OpenGL program object
        glUseProgram(gShaderProgramObject_PV);

        if (bLight == true)
        {
            fprintf(gpFile, "%d\n", __LINE__);
            glUniform1i(LKeyPressedUniform, 1);
            glUniform3fv(LaUniform_PV, 1, (GLfloat*)LaUniforms);
            glUniform3fv(LdUniform_PV, 1, (GLfloat*)LdUniforms);
            glUniform3fv(LsUniform_PV, 1, (GLfloat*)LsUniforms);

            glUniform3fv(KaUniform_PV, 1, (GLfloat*)KaUniforms);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)KdUniforms);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)KsUniforms);

            glUniform1f(materialShineness_PV, 128.0f);
        }
        else
        {
            glUniform1i(LKeyPressedUniform, 0);
        }
        glUniform4fv(lightPositionUniform_PV, 1, (GLfloat*)lightPosition);      // It sends vector of 4 floats - sending light in an array - to send in array format use this glUniform4fv() function.

        ModelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

        glUniformMatrix4fv(gModelMatrix_PV, 1, GL_FALSE, ModelMatrix);
        glUniformMatrix4fv(gViewMatrix_PV, 1, GL_FALSE, ViewMatrix);
        glUniformMatrix4fv(gProjectionMatrix_PV, 1, GL_FALSE, gPerspectiveProjectionMatrix);
    }

    else if (chooseShader == 2)
    {
        glUseProgram(gShaderProgramObject_PF);

        if (bLight == true)
        {
            glUniform1i(LKeyPressedUniform, 1);
            glUniform3fv(LaUniform_PF, 1, (GLfloat*)LaUniforms);
            glUniform3fv(LdUniform_PF, 1, (GLfloat*)LdUniforms);
            glUniform3fv(LsUniform_PF, 1, (GLfloat*)LsUniforms);

            glUniform3fv(KaUniform_PF, 1, (GLfloat*)KaUniforms);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)KdUniforms);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)KsUniforms);

            glUniform1f(materialShineness_PF, 128.0f);
        }
        else
        {
            glUniform1i(LKeyPressedUniform, 0);
        }
        glUniform4fv(lightPositionUniform_PF, 1, (GLfloat*)lightPosition);      // It sends vector of 4 floats - sending light in an array - to send in array format use this glUniform4fv() function.

        
        ModelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

        glUniformMatrix4fv(gModelMatrix_PF, 1, GL_FALSE, ModelMatrix);
        glUniformMatrix4fv(gViewMatrix_PF, 1, GL_FALSE, ViewMatrix);
        glUniformMatrix4fv(gProjectionMatrix_PF, 1, GL_FALSE, gPerspectiveProjectionMatrix);
    }


    // *** bind vao ***
    // all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao_sphere);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

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