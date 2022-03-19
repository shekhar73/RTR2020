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
    GLuint gVbo_Texture_Cube;

    GLuint gMVPUniform;

    #define CHECK_IMAGE_WIDTH    64
    #define CHECK_IMAGE_HEIGHT    64

    GLubyte CheckImage[CHECK_IMAGE_HEIGHT][CHECK_IMAGE_WIDTH][4];
    GLuint texImage;
    GLuint textureSamplerUniform;

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

    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec2 vTexCoord;" \
        "uniform mat4 u_mvp_matrix;" \
        "out vec2 out_TexCoord;" \
        "void main(void)" \
        "\n" \
        "{" \
            "gl_Position = u_mvp_matrix * vPosition;" \
            "out_TexCoord = vTexCoord;" \
        "}";
    
    glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    glCompileShader(gVertexShaderObject);

    GLint iInfoLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szBuffer_InfoLog = NULL;
    glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)    //Check
    {
        glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szBuffer_InfoLog = (char*)malloc(sizeof(char) * iInfoLength);
            if (szBuffer_InfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szBuffer_InfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szBuffer_InfoLog);
                free(szBuffer_InfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    fprintf(gpFile, "Complie Vertex shader Successful.\n");

    ///////////////////////////////////////////////////////////////////////////////////////
    //*** FRAGMENT SHADER ***
    ///////////////////////////////////////////////////////////////////////////////////////

    // Create shader
    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    //Provide Source Code to Shader
    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec2 out_TexCoord;" \
        "uniform sampler2D u_texture_sampler;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "\n" \
        "{" \
            "FragColor = texture(u_texture_sampler,out_TexCoord);" \
        "}";

    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    glCompileShader(gFragmentShaderObject);


    iInfoLength = 0;
    iShaderCompiledStatus = 0;
    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szBuffer_InfoLog = (char*) malloc(sizeof(char) * iInfoLength);
            if (szBuffer_InfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szBuffer_InfoLog);
                fprintf(gpFile, "Fragment Shader Compiled Log : %s\n", szBuffer_InfoLog);
                free(szBuffer_InfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    fprintf(gpFile, "Complie Fragment shader Successful.\n");

    /////////////////////////////////////////////////////////////////////////////////////
    //*** SHADER PROGRAM ***
    /////////////////////////////////////////////////////////////////////////////////////

    // create
    gShaderProgramObject = glCreateProgram();

    //Attach Vertex Shader to Shader Program
    glAttachShader(gShaderProgramObject, gVertexShaderObject);

    //Attach Fragment Shader to Shader Program
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    fprintf(gpFile, "Attach shader Successful.\n");

    // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXCOORD, "vTexCoord");

    //link shader
    glLinkProgram(gShaderProgramObject);
    fprintf(gpFile, "link shader Successful.\n");

    // Reset values
    GLint iShaderProgramLinkStatus = 0;
    iInfoLength = 0;
    
    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
        if (iInfoLength > 0)
        {
            szBuffer_InfoLog = (char*)malloc(sizeof(char) * iInfoLength);
            if (szBuffer_InfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szBuffer_InfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szBuffer_InfoLog);
                free(szBuffer_InfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // get MVP uniform location
    // post linking
    gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
    textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
    

    glGenVertexArrays(1, &gVao_Cube);
    glBindVertexArray(gVao_Cube);
        glGenBuffers(1, &gVbo_Position_Cube);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenBuffers(1, &gVbo_Texture_Cube);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Cube);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0,NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




   // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_TEXTURE_2D);
    [self loadGLTextures];

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


//Texture
-(void)loadGLTextures
{
    [self MakeCheck];

    glGenTextures(1, &texImage);
    glBindTexture(GL_TEXTURE_2D, texImage);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, CheckImage);
    
}

-(void)MakeCheck
{
    int i, j, k;

    for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
    {
        for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
        {
            k = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

            CheckImage[i][j][0] = (GLubyte)k;
            CheckImage[i][j][1] = (GLubyte)k;
            CheckImage[i][j][2] = (GLubyte)k;
            CheckImage[i][j][3] = (GLubyte)255;
        }
    }

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

    GLfloat cubeVertices[12];
    GLfloat TexCoord[8];
    
    glUseProgram(gShaderProgramObject);

    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 translateMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix  = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

    translateMatrix = vmath::translate(0.0f,0.0f,-6.0f);
    scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
   
    modelViewMatrix = translateMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glUniform1i(textureSamplerUniform, 0);

    glBindVertexArray(gVao_Cube);
        cubeVertices[0] = -2.0f;
        cubeVertices[1] = -1.0f;
        cubeVertices[2] = 0.0f;
        cubeVertices[3] = -2.0f;
        cubeVertices[4] = 1.0f;
        cubeVertices[5] = 0.0f;
        cubeVertices[6] = 0.0f;
        cubeVertices[7] = 1.0f;
        cubeVertices[8] = 0.0f;
        cubeVertices[9] = 0.0f;
        cubeVertices[10] = -1.0f;
        cubeVertices[11] = 0.0f;

        TexCoord[0] = 0.0;
        TexCoord[1] = 0.0;
        TexCoord[2] = 0.0;
        TexCoord[3] = 1.0;
        TexCoord[4] = 1.0;
        TexCoord[5] = 1.0;
        TexCoord[6] = 1.0;
        TexCoord[7] = 0.0;
    
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), cubeVertices, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Cube);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), TexCoord, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //Tilted Quad
        cubeVertices[0] = 1.0f;
        cubeVertices[1] = -1.0f;
        cubeVertices[2] = 0.0f;
        cubeVertices[3] = 1.0f;
        cubeVertices[4] = 1.0f;
        cubeVertices[5] = 0.0f;
        cubeVertices[6] = 2.41421f;
        cubeVertices[7] = 1.0f;
        cubeVertices[8] = -1.41421f;
        cubeVertices[9] = 2.41421f;
        cubeVertices[10] = -1.0f;
        cubeVertices[11] = -1.41421f;

        TexCoord[0] = 0.0;
        TexCoord[1] = 0.0;
        TexCoord[2] = 0.0;
        TexCoord[3] = 1.0;
        TexCoord[4] = 1.0;
        TexCoord[5] = 1.0;
        TexCoord[6] = 1.0;
        TexCoord[7] = 0.0;

        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), cubeVertices, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Cube);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), TexCoord, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
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