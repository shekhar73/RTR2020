#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"
#import "GLESView.h"
using namespace vmath;

enum {
    SSK_ATTRIBUTE_POSITION  = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXCORD
};

@implementation GLESView
{
    EAGLContext *eaglContext;
    GLuint defaultFramebuffer;
    GLuint depthRenderBuffer;
    GLuint colorRenderBuffer;
  
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[746];
    unsigned short sphere_elements[2280];
    int gNumSphereVertices;
    int gNumSphereElements;

    int COUNTER_MAX;
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;

    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_normal;
    GLuint vbo_texture;
    GLuint vbo_element;


    GLuint modelMatrixUniform;
    GLuint perspectiveProjectionUniform;
    GLuint viewMatrixUniform;


    mat4 perspectiveProjectionMatrix;
    
    mat4 STACK_MATRIX[32];
    int iCOUNTER;
    GLfloat rotateEarth;
    GLfloat revolveEarth;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
}

-(id) initWithFrame:(CGRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys :
                                        [NSNumber numberWithBool:FALSE],
                                        kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (eaglContext == NULL) {
            [self release];
            return (nil);
        }
         
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1, &defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderBuffer);
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        glGenRenderbuffers(1, &depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Failed to create complete framebuffer object %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            
            glDeleteFramebuffers(1, &defaultFramebuffer);
            glDeleteRenderbuffers(1, &colorRenderBuffer);
            glDeleteRenderbuffers(1, &depthRenderBuffer);
            
            [self release];
            return (nil);
        }
        
        printf("Renderer: %s | GL Version: %s | GLSL Version: %s\n",
               glGetString(GL_RENDERER), glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
        

       
        COUNTER_MAX = 32;
        iCOUNTER = -1;
        rotateEarth = 0.0f;
        revolveEarth = 0.0f;
        
        
    
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

        const GLchar* vertexShaderSourceCode =
        "#version 300 core" \
        "\n" \
        "in vec4 vPosition;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "void main(void)" \
        "{"\
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
        "}";
        
        glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
        glCompileShader(gVertexShaderObject);
        
        GLint infoLogLength = 0;
        GLint shaderCompileStatus = 0;
        char* szBuffer = NULL;


        glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);
        if (shaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
                if (szBuffer != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject, infoLogLength, &written, szBuffer);

                    printf("Vertex Shader Per Vertex Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }

        printf("Vertex Shader Done\n");
        

    gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        

    const GLchar* fragmentShaderSourceCode =
        "#version 300 core" \
        "\n" \
        "precision highp float;"\
        "in vec3 out_Color;"
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
        "}";
    
    glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    glCompileShader(gFragmentShaderObject);

    infoLogLength = 0;
    shaderCompileStatus = 0;
    szBuffer = NULL;
    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus == GL_FALSE) {
        glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
            if (szBuffer != NULL) {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject, infoLogLength, &written, szBuffer);

                printf("Fragment Shader Per Vertex Compilation Log : %s\n", szBuffer);
                free(szBuffer);
                [self release];
            }
        }
    }

    gShaderProgramObject = glCreateProgram();

        glAttachShader(gShaderProgramObject, gVertexShaderObject);
        glAttachShader(gShaderProgramObject, gFragmentShaderObject);
        glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");

    glLinkProgram(gShaderProgramObject);
    infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    szBuffer = NULL;
    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);
    if (shaderProgramLinkStatus == GL_FALSE) {
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
            if (szBuffer != NULL) {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject, infoLogLength, &written, szBuffer);

                printf("Shader Program Log : %s\n", szBuffer);
                free(szBuffer);
                [self release];
            }
        }
    }

        
        modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_modelMatrix");
        viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_viewMatrix");
        perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");


        getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        gNumSphereVertices = getNumberOfSphereVertices();
        gNumSphereElements = getNumberOfSphereElements();


        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_element);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        glBindVertexArray(0);

        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glClearColor(0.2f,0.2f,0.2f,1.0f);


    // ortho or perspective
    perspectiveProjectionMatrix = mat4::identity();
            
                
        // Gesture Recognition
        UITapGestureRecognizer *singleTapGestureRecognizer =
        [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        
        [singleTapGestureRecognizer setNumberOfTapsRequired: 1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer =
        [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        
        [doubleTapGestureRecognizer setNumberOfTapsRequired: 2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer =
        [[UISwipeGestureRecognizer alloc]initWithTarget: self action:@selector (onSwipe:)];
        
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer =
        [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    
    return (self);
 
}

- (void)drawRect:(CGRect)rect {
    // Drawing code
}

+(Class)layerClass {
    // code
    return([CAEAGLLayer class]);
}

-(void)drawView:(id)sender 
{
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
     
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gShaderProgramObject);

    mat4 translateMatrix = mat4::identity();
    mat4 rotationMatrixX = mat4::identity();
    mat4 rotationMatrixY = mat4::identity();
    mat4 rotationMatrixZ = mat4::identity();
    mat4 scaleMatrix = mat4::identity();

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translateMatrix = vmath::translate(0.0f, 0.0f, -12.0f);
    modelMatrix = translateMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    [self pushMatrix:modelMatrix];

    rotationMatrixY = vmath::rotate(rotateEarth, 0.0f, 1.0f, 0.0f);
    modelMatrix = modelMatrix * rotationMatrixY;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    [self pushMatrix:modelMatrix];
        scaleMatrix = vmath::scale(3.0f, 3.0f, 3.0f);
        modelMatrix = modelMatrix * scaleMatrix;
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    modelMatrix = [self popMatrix];


    translateMatrix = vmath::translate(4.5f, 0.0f, 0.0f);
    rotationMatrixZ = vmath::rotate(revolveEarth, 0.0f, 1.0f, 0.0f);
    modelMatrix = modelMatrix * translateMatrix * rotationMatrixZ;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    [self pushMatrix:modelMatrix];
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
        glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    modelMatrix = [self popMatrix];


    modelMatrix = [self popMatrix];
    glUseProgram(0);
    
 
    

    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)layoutSubviews {
    // code
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    [eaglContext renderbufferStorage: GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    
    glViewport(0, 0, width, height);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Failed to create complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }

    perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);
    
    [self drawView:nil];
}

-(void)pushMatrix:(mat4)matrix{

    if (iCOUNTER < COUNTER_MAX-1) {
        iCOUNTER++;
        STACK_MATRIX[iCOUNTER] = matrix;
    }
    else {
        printf("Stack Overflow.\n");
    }
}

-(mat4)popMatrix{
    if (iCOUNTER > -1) {
        int temp = iCOUNTER;
        iCOUNTER--;
        return STACK_MATRIX[temp];
    }
    else{
        printf("Stack Underflow.\n");
        return mat4::identity();
    }
}

-(void)startAnimation {
    if (!isAnimating) {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        
        isAnimating = true;
    }
}

-(void)stopAnimation {
    if (!isAnimating) {
        [displayLink invalidate];
        displayLink = nil;
        
        isAnimating = NO;
    }
}
        

-(BOOL)acceptsFirstResponder {
    // code
    return (YES);
}

-(void)touchesBegan:(NSSet *)touches withEvent: (UIEvent *)event {
    // code

}

-(void)onSingleTap:(UITapGestureRecognizer *)gr {
     rotateEarth = rotateEarth + 0.5f;
   
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr {
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr {
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr {
     revolveEarth = revolveEarth + 0.5f;
}

- (void)dealloc {

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo_position) {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }

    if (vbo_normal) {
        glDeleteBuffers(1, &vbo_normal);
        vbo_normal = 0;
    }

    if (vbo_element) {
        glDeleteBuffers(1, &vbo_element);
        vbo_element = 0;
    }

    
    // code
    if (depthRenderBuffer) {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
        depthRenderBuffer = 0;
    }
    
    if (colorRenderBuffer) {
        glDeleteRenderbuffers(1, &colorRenderBuffer);
        colorRenderBuffer = 0;
    }
    
    if (defaultFramebuffer) {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
    
    if ([EAGLContext currentContext] == eaglContext) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [eaglContext release];
    eaglContext = nil;
    
    [super dealloc];
}
@end
