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

    GLuint gVao_Pyramid;
    GLuint gVbo_Position_Pyramid;
    GLuint gVbo_Color_Pyramid;

    GLuint gVao_Cube;
    GLuint gVbo_Position_Cube;

    GLuint gMVPUniform;
    
    
    //lights
    GLuint laUniform;
    GLuint ldUniform;
    GLuint lsUniform;
    GLuint lightPositionUniform;

    //material
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLfloat shininessUniform;
    GLuint LKeyPressedUniform;

    //texture
    GLuint marble_texture;
    GLuint textureSamplerUniform;

    //Animation
    GLfloat angleCube;

    //Toggle Variable
    BOOL bLight;
    
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
    
    // Provide source code to shader
    const GLchar* vertexShaderSourceCode =
    "#version 410 core" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec4 vColor;" \
    "in vec2 vTexCoord;" \
    "in vec3 vNormal;" \
    "uniform vec3 u_la;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_ls;" \
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    "uniform vec4 u_lightPosition;" \
    "uniform mat4 u_mvp_matrix;" \
    "uniform int u_LKeyPressed;" \
    "uniform float u_materialShininess;" \
    "uniform int u_keyPressed;" \
    "out vec4 out_color;" \
    "out vec2 out_TexCoord;" \
    "out vec3 phong_ads_light;" \
    "void main(void)" \
    "\n" \
    "{" \
        "if(u_LKeyPressed == 1)" \
        "{" \
            "vec4 eye_coordinate = u_mvp_matrix * vPosition;" \
            "vec3 transformed_normal = normalize(mat3(u_mvp_matrix) * vNormal);" \
            "vec3 light_direction = normalize(vec3(u_lightPosition - eye_coordinate));" \
            "vec3 reflection_vector = reflect(-light_direction,transformed_normal);" \
            "vec3 view_vector = normalize(-eye_coordinate.xyz);" \
            "vec3 ambient = u_la * u_ka;" \
            "vec3 diffuse = u_ld * u_kd * max(dot(light_direction,transformed_normal),0.0);" \
            "vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,view_vector),0.0),u_materialShininess);" \
            "phong_ads_light = ambient + diffuse + specular;" \
        "}" \
        "else" \
        "{" \
            "phong_ads_light = vec3(1.0,1.0,1.0);" \
        "}" \
        "gl_Position = u_mvp_matrix * vPosition;" \
        "out_color = vColor;"
        "out_TexCoord = vTexCoord;" \
    "}";

    glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    glCompileShader(gVertexShaderObject);

    GLint iInfoLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;

    // shader compilation error checking

    // void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
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

    const GLchar* fragmentShaderSourceCode =
    "#version 410 core" \
    "\n" \
    "in vec3 phong_ads_light;" \
    "in vec4 out_color;" \
    "in vec2 out_TexCoord;" \
    "uniform sampler2D u_texture_sampler;" \
    "uniform int u_keyPressed;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "\n" \
    "{" \
        "vec3 tex = vec3(texture(u_texture_sampler,out_TexCoord));" \
        "FragColor = vec4(vec3(out_color) * phong_ads_light * tex,1.0);" \
    "}";

    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    
        // compile shader

    glCompileShader(gFragmentShaderObject);

    // fragment shader compilation error checking

    iInfoLength = 0;
    iShaderCompiledStatus = 0;
    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szInfoLog = (char*) malloc(sizeof(char) * iInfoLength);
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

    //attach Vertex Shader to Shader Program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    //attach Fragment Shader to Shader Program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    fprintf(gpFile, "Attach shader Successful.\n");

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the 'in' and 'uniform' attributes
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXCOORD, "vTexCoord");
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
    gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
    textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
    laUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    lsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_lightPosition");
    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_materialShininess");
    LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");



    const GLfloat cube_PCNT[] =
    {
        //front
        1.0f,1.0f,1.0f,            1.0f,0.0f,0.0f,        0.0f,0.0f,1.0f,        0.0f,0.0f,
        -1.0f,1.0f,1.0f,        1.0f,0.0f,0.0f,        0.0f,0.0f,1.0f,        1.0f, 0.0f,
        -1.0f,-1.0f,1.0f,        1.0f,0.0f,0.0f,        0.0f,0.0f,1.0f,        1.0f, 1.0f,
        1.0f,-1.0f,1.0f,        1.0f,0.0f,0.0f,        0.0f,0.0f,1.0f,        0.0f, 1.0f,

        //right
        1.0f,1.0f,-1.0f,        0.0f,1.0f,0.0f,        1.0f, 0.0f, 0.0f,    0.0f,0.0f,
        1.0f,1.0f,1.0f,            0.0f,1.0f,0.0f,        1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        1.0f,-1.0f,1.0f,        0.0f,1.0f,0.0f,        1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,        0.0f,1.0f,0.0f,        1.0f, 0.0f, 0.0f,    0.0f, 1.0f,

        //back
        -1.0f,1.0f,-1.0f,       0.0f,0.0f,1.0f,        0.0f, 0.0f, -1.0f,    1.0f, 0.0f,
        1.0f,1.0f,-1.0f,        0.0f,0.0f,1.0f,        0.0f, 0.0f, -1.0f,    1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,        0.0f,0.0f,1.0f,        0.0f, 0.0f, -1.0f,    0.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,        0.0f,0.0f,1.0f,        0.0f, 0.0f, -1.0f,    0.0f,0.0f,

        //left
        -1.0f, 1.0f, 1.0f,        1.0f,1.0f,0.0f,        -1.0f, 0.0f, 0.0f,    0.0f,0.0f,
        -1.0f, 1.0f, -1.0f,        1.0f,1.0f,0.0f,        -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,0.0f,        -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,        1.0f,1.0f,0.0f,        -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,

        //top
        1.0f,1.0f,-1.0f,        0.0f,1.0f,1.0f,        0.0f, 1.0f, 0.0f,    0.0f,0.0f,
        -1.0f,1.0f,-1.0f,        0.0f,1.0f,1.0f,        0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        -1.0f,1.0f,1.0f,        0.0f,1.0f,1.0f,        0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        1.0f,1.0f,1.0f,            0.0f,1.0f,1.0f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,

        //bottom
        1.0f,-1.0f,-1.0f,        1.0f,0.0f,1.0f,        0.0f, -1.0f, 0.0f,    0.0f,0.0f,
        -1.0f,-1.0f,-1.0f,        1.0f,0.0f,1.0f,        0.0f, -1.0f, 0.0f,    1.0f, 0.0f,
        -1.0f,-1.0f,1.0f,        1.0f,0.0f,1.0f,        0.0f, -1.0f, 0.0f,    1.0f, 1.0f,
        1.0f,-1.0f,1.0f,        1.0f,0.0f,1.0f,        0.0f, -1.0f, 0.0f,    0.0f, 1.0f
    };

    glGenVertexArrays(1, &gVao_Cube);
    glBindVertexArray(gVao_Cube);
    glGenBuffers(1, &gVbo_Position_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_PCNT), cube_PCNT, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);

    glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),  (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);

    glVertexAttribPointer(SSK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));
    glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    marble_texture = [self loadGLTextures:"Marble.bmp"];
    
    glClearColor(0.0f, 0.0f, 0.10f, 0.0f);

    gPerspectiveProjectionMatrix = mat4::identity();

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

//Texture
-(GLuint)loadGLTextures:(const char*)imageFileName
{
    //code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *imageFileNameWithPath = [NSString stringWithFormat:@"%@/%s",parentDirPath,imageFileName];    
    NSImage *bmpImage = [[NSImage alloc]initWithContentsOfFile:imageFileNameWithPath];

    if(!bmpImage){
        fprintf(gpFile,"Failed to conversion of image file\n");
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

        case 'l':
        case 'L':
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

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
        glUniform1i(LKeyPressedUniform, 1);

        GLfloat lightAmbient[]      = {0.0f, 0.0f, 0.0f};
        GLfloat lightDiffuse[]      = {1.0f, 1.0f, 1.0f};
        GLfloat lightSpecular[]     = {1.0f, 1.0f, 1.0f};

        GLfloat materialAmbient[]   = {0.0f, 0.0f, 0.0f};
        GLfloat materialDiffuse[]   = {1.0f, 1.0f, 1.0f};
        GLfloat materialSpecular[]  = {1.0f, 1.0f, 1.0f};

        GLfloat lightPosition[]     = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat materialShininess   = 128.0f;


        glUniform3fv(laUniform, 1, (GLfloat*)lightAmbient);
        glUniform3fv(ldUniform, 1, (GLfloat*)lightDiffuse);
        glUniform3fv(lsUniform, 1, (GLfloat*)lightSpecular);
        glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

        glUniform3fv(kaUniform, 1, (GLfloat*)materialAmbient);
        glUniform3fv(kdUniform, 1, (GLfloat*)materialDiffuse);
        glUniform3fv(ksUniform, 1, (GLfloat*)materialSpecular);
        glUniform1f(shininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    mat4 modelViewMatrix = mat4::identity();
    mat4 translateMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix  = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f,0.0f,-6.0f);
    scaleMatrix = scale(0.75f, 0.75f, 0.75f);
    rotationMatrix  = rotate(angleCube,1.0f,0.0f,0.0f);
    rotationMatrix  = rotationMatrix * rotate(angleCube,0.0f,1.0f,0.0f);
    rotationMatrix  = rotationMatrix * rotate(angleCube,0.0f,0.0f,1.0f);
    modelViewMatrix = translateMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, marble_texture);
    glUniform1i(textureSamplerUniform, 0);

    glBindVertexArray(gVao_Cube);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);

    glUseProgram(0);

    
    angleCube = angleCube + 0.50f;
    if(angleCube >= 360)
    {
        angleCube = 0.0f;
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
