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

    // pyramid
    GLuint gVao_Pyramid;
    GLuint gVbo_Position_Pyramid;
    GLuint gVbo_Texture_Pyramid;

    // cube
    GLuint gVao_Cube;
    GLuint gVbo_Position_Cube;
    GLuint gVbo_Texture_Cube;

    GLuint gMVPUniform;

    mat4 gPerspectiveProjectionMatrix;

    GLfloat pyramidAngle = 0.0f;
    GLfloat cubeAngle = 0.0f;

    GLuint stone_texture;
    GLuint kundali_texture;
    GLuint gTextureSamplerUniform;
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

    //provide source code to shader
    const GLchar* vertexShaderSourceCode =
        "#version 450 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec2 vTexture;" \
        "uniform mat4 u_mvp_matrix;" \
        "out vec2 out_texture;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvp_matrix * vPosition;" \
        "out_texture = vTexture;" \
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
    const GLchar* fragmentShaderSourceCode =
        "#version 450 core" \
        "\n" \
        "in vec2 out_texture;" \
        "uniform sampler2D u_texture_sampler;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = texture(u_texture_sampler, out_texture);" \
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

    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXTURE, "vTexture");

    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");

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

    // get MVP uniform location
    gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
    gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

    const GLfloat pyramidVertices[] = { 0.0f, 0.5f, 0.0f,
                                        -0.5f, -0.5f, 0.5f,
                                        0.5f, -0.5f, 0.5f,

                                        0.0f, 0.5f, 0.0f,
                                        -0.5f, -0.5f, -0.5f,
                                        -0.5f, -0.5f, 0.5f,

                                        0.0f, 0.5f, 0.0f,
                                        0.5f, -0.5f, -0.5f,
                                        -0.5f, -0.5f, -0.5f,

                                        0.0f, 0.5f, 0.0f,
                                        0.5f, -0.5f, 0.5f,
                                        0.5f, -0.5f, -0.5f };

    const GLfloat texturePyramid[] = { 0.5f, 1.0f,
                                        0.0f, 0.0f,
                                        1.0f, 0.0f,

                                        0.5f, 1.0f,
                                        1.0f, 0.0f,
                                        0.0f, 0.0f,

                                        0.5f, 1.0f,
                                        1.0f, 0.0f,
                                        0.0f, 0.0f,

                                        0.5f, 1.0f,
                                        0.0f, 0.0f,
                                        1.0f, 0.0f };

    const GLfloat cubeVertices[] = {
                                    //Front View
                                    0.5f, 0.5f, 0.5f,
                                    -0.5f, 0.5f, 0.5f,
                                    -0.5f, -0.5f, 0.5f,
                                    0.5f, -0.5f, 0.5f,

                                    //Right Side
                                    0.5f, 0.5f, -0.5f,
                                    0.5f, 0.5f, 0.5f,                                   
                                    0.5f, -0.5f, 0.5f,
                                    0.5f, -0.5f, -0.5f,

                                    //Back Side
                                    -0.5f, 0.5f, -0.5f,
                                    0.5f, 0.5f, -0.5f,
                                    0.5f, -0.5f, -0.5f,
                                    -0.5f, -0.5f, -0.5f,

                                    //Left Side
                                    -0.5f, 0.5f, 0.5f,
                                    -0.5f, 0.5f, -0.5f,
                                    -0.5f, -0.5f, -0.5f,
                                    -0.5f, -0.5f, 0.5f,

                                    //Top Side
                                    0.5f, 0.5f, -0.5f,
                                    -0.5f, 0.5f, -0.5f,
                                    -0.5f, 0.5f, 0.5f,
                                    0.5f, 0.5f, 0.5f,

                                    //Bottom Side
                                    0.5f, -0.5f, -0.5f,
                                    -0.5f, -0.5f, -0.5f,
                                    -0.5f, -0.5f, 0.5f,
                                    0.5f, -0.5f, 0.5f };

    const GLfloat textureCube[] = { 
                                    //Front View
                                    0.0f, 0.0f,
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,
                                    0.0f, 1.0f,

                                    //Right Side
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,
                                    0.0f, 1.0f,
                                    0.0f, 0.0f,

                                    //Back Side                                 
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,
                                    0.0f, 1.0f,
                                    0.0f, 0.0f,

                                    //Left Side
                                    0.0f, 0.0f,
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,
                                    0.0f, 1.0f,

                                    //Top Side
                                    0.0f, 1.0f,
                                    0.0f, 0.0f,
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,

                                    //Bottom Side
                                    1.0f, 1.0f,
                                    0.0f, 1.0f,
                                    0.0f, 0.0f,
                                    1.0f, 0.0f
    };

    glGenVertexArrays(1, &gVao_Pyramid);
    glBindVertexArray(gVao_Pyramid);

    glGenBuffers(1, &gVbo_Position_Pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &gVbo_Texture_Pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturePyramid), texturePyramid, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glGenVertexArrays(1, &gVao_Cube);
    glBindVertexArray(gVao_Cube);

    glGenBuffers(1, &gVbo_Position_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &gVbo_Texture_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCube), textureCube, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


   // set-up detpth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
        glEnable(GL_TEXTURE_2D);
   
    stone_texture = [self loadGLTextures:"Stone.bmp"];
    kundali_texture    = [self loadGLTextures:"Kundali.bmp"];

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



-(GLuint)loadGLTextures:(const char*)imageFileName
{
    //code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *imageFileNameWithPath = [NSString stringWithFormat:@"%@/%s",parentDirPath,imageFileName];
    NSImage *bmpImage = [[NSImage alloc]initWithContentsOfFile:imageFileNameWithPath];

    if(!bmpImage){
        fprintf(gpFile,"NSImage conversion of image file failed\n");
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

  // start using opengl program object
    glUseProgram(gShaderProgramObject);

    // opengl drawing
    // set modelview & modelviewprojection matrix to identity
    mat4 TranslateMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
    mat4 RotationMatrix = vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);
    mat4 modelViewMatrix = TranslateMatrix * RotationMatrix;
    mat4 modelViewProjectionMatrix = mat4::identity();

    // multiply modelview and perspective matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // order is important

    //pass above modelviewprojection matrix to the vertex shader in 'u_mvpMatrix' shader variable
    // whose position value we already calculated in intiWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stone_texture);
    glUniform1i(gTextureSamplerUniform, 0);
    // bind vao
    glBindVertexArray(gVao_Pyramid);

    // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLES, 0, 12); // 12 total vertex

    // unbind vao
    glBindVertexArray(0);

    //stop using opengl program object
    // glUseProgram(0);

    TranslateMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
    mat4 ScaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
    RotationMatrix = vmath::rotate(cubeAngle, 1.0f, 1.0f, 1.0f);
    

    //opengl drawing
    // set modelview & modelviewprojection matrix to identity
    modelViewMatrix = TranslateMatrix * ScaleMatrix * RotationMatrix;
    modelViewProjectionMatrix = mat4::identity();

    // multiply modelview and orthographic matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; // order is important

    // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
    // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, kundali_texture);
    glUniform1i(gTextureSamplerUniform, 0);

    // bind vao
    glBindVertexArray(gVao_Cube);

    // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);    // 4 for 4 vertex
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

    // unbind vao
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // stop using opengl program object
    glUseProgram(0);

    pyramidAngle += 0.5f;
    if (pyramidAngle >= 360.0f)
        pyramidAngle = 0.0f;

    cubeAngle += 0.5f;
    if (cubeAngle >= 360.0f)
        cubeAngle = 0.0f;


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