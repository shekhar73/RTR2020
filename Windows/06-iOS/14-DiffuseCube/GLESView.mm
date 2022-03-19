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
  
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;
    
    GLuint vao_pyramid;
    GLuint vbo_position_pyramid;
    GLuint vbo_color_pyramid;

    GLuint vao_cube;
    GLuint vbo_cube_position;
    GLuint vbo_cube_normal;

    GLfloat rotation_X_Axis;
    GLfloat rotation_Y_Axis;
    GLfloat rotation_Z_Axis;

    BOOL bAnimate;
    BOOL bLight;

    GLuint modelViewMatrixUniform;
    GLuint perspectiveProjectionUniform;
    GLuint lKeyPressedUniform;
    GLuint LdUniform;
    GLuint KdUniform;
    GLuint LightPositionUniform;
    mat4 perspectiveProjectMatrix;
    
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
        
        rotation_X_Axis = 0.0f;
        rotation_Y_Axis = 0.0f;
        rotation_Z_Axis = 0.0f;

        bAnimate = NO;
        bLight = NO;
        
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        const GLchar* vertexShaderSourceCode =
            "#version 300 core" \
            "\n" \
            "in vec4 vPosition;" \
            "in vec3 vNormal;" \
            "uniform mat4 u_modelViewMatrix;" \
            "uniform mat4 u_projectionMatrix;" \
            "uniform int u_lKeyPressed;" \
            "uniform vec3 u_ld;" \
            "uniform vec3 u_kd;" \
            "uniform vec4 light_position;" \
            "out vec3 diffuse_light;" \
            "void main(void)" \
            "{" \
            "if(u_lKeyPressed == 1){" \
            "vec4 eyeCoordinate = u_modelViewMatrix * vPosition;" \
            "mat3 normal_matrix = mat3(transpose(inverse(u_modelViewMatrix)));" \
            "vec3 tNormal = normalize(normal_matrix * vNormal);" \
            "vec3 s = normalize(vec3(light_position - eyeCoordinate));" \
            "diffuse_light = u_ld * u_kd * max(dot(s, tNormal) , 0.0);" \
            "gl_Position = u_projectionMatrix * u_modelViewMatrix * vPosition;" \
            "}"\
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

                    printf("Vertex Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);

                    [self release];
                }
            }
        }


        gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

        const GLchar* fragmentShaderSourceCode =
            "#version 300 core" \
            "\n" \
            "in highp vec3 diffuse_light;" \
            "uniform highp int u_lKeyPressed;" \
            "out highp vec4 FragColor;" \
            "void main(void)" \
            "{" \
            "highp vec4 color;" \
            "if(u_lKeyPressed == 1){" \
            "color = vec4(diffuse_light , 1.0f);" \
            "}" \
            "else{" \
            "color = vec4(1.0, 1.0, 1.0, 1.0);" \
            "}" \
            "FragColor = color;" \
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

                    printf("Fragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);

                    [self release];
                }
            }
        }

        gShaderProgramObject = glCreateProgram();

        glAttachShader(gShaderProgramObject, gVertexShaderObject);

        glAttachShader(gShaderProgramObject, gFragmentShaderObject);
        glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");
        

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

        modelViewMatrixUniform            = glGetUniformLocation(gShaderProgramObject, "u_modelViewMatrix");
        perspectiveProjectionUniform    = glGetUniformLocation(gShaderProgramObject, "u_projectionMatrix");
        lKeyPressedUniform                = glGetUniformLocation(gShaderProgramObject, "u_lKeyPressed");
        LdUniform                        = glGetUniformLocation(gShaderProgramObject, "u_ld");
        KdUniform                        = glGetUniformLocation(gShaderProgramObject, "u_kd");
        LightPositionUniform            = glGetUniformLocation(gShaderProgramObject, "light_position");
      
        const GLfloat cubeVertices[] = {
            // Front
            1.0f,1.0f,1.0f,
            -1.0f,1.0f,1.0f,
            -1.0f,-1.0f,1.0f,
            1.0f,-1.0f,1.0f,

            // Right
            1.0f,1.0f,-1.0f,
            1.0f,1.0f,1.0f,
            1.0f,-1.0f,1.0f,
            1.0f,-1.0f,-1.0f,

            // Back
            -1.0f,1.0f,-1.0f,
            1.0f,1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,

            // Left
            -1.0f,1.0f,1.0f,
            -1.0f,1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,1.0f,

            // Top
            1.0f,1.0f,-1.0f,
            -1.0f,1.0f,-1.0f,
            -1.0f,1.0f,1.0f,
            1.0f,1.0f,1.0f,

            // Bottom
            -1.0f,-1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f,-1.0f,1.0f,
            -1.0f,-1.0f,1.0f,
        };

        const GLfloat cubeNormal[] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,

            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
        };

        glGenVertexArrays(1, &vao_cube);
        glBindVertexArray(vao_cube);

        glGenBuffers(1, &vbo_cube_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_cube_normal);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormal), cubeNormal, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        
        perspectiveProjectMatrix = mat4::identity();
        
            
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

-(void)drawView:(id)sender {
    // code
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gShaderProgramObject);

    GLfloat lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};

    if (bLight == YES) {
        glUniform1i(lKeyPressedUniform , 1);
        glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f);
        glUniform4fv(LightPositionUniform, 1, lightPosition);
    }
    else 
    {
        glUniform1i(lKeyPressedUniform, 0);
    }

    mat4 translateMatrix = mat4::identity();
    mat4 rotationMatrixX = mat4::identity();
    mat4 rotationMatrixY = mat4::identity();
    mat4 rotationMatrixZ = mat4::identity();

    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translateMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrixX = vmath::rotate(rotation_X_Axis, 1.0f, 0.0f, 0.0f);
    rotationMatrixY = vmath::rotate(rotation_Y_Axis, 0.0f, 1.0f, 0.0f);
    rotationMatrixZ = vmath::rotate(rotation_Z_Axis, 0.0f, 0.0f, 1.0f);

    modelViewMatrix = translateMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ;
    glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);

    modelViewProjectionMatrix = perspectiveProjectMatrix;
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glBindVertexArray(vao_cube);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);

    glUseProgram(0);

    if (bAnimate == YES) {
        rotation_X_Axis += 0.5f;
        rotation_Y_Axis += 0.5f;
        rotation_Z_Axis += 0.5f;
    }    


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
    // code
   
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr {
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr {
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr {
   
}

- (void)dealloc {

    
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
