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
  
    GLuint vao_pyramid;
    GLuint vbo_pyramid_position;
    GLuint vbo_pyramid_normal;


    BOOL bAnimate;
    BOOL bLight;


    GLuint gVertexShaderObjectPV;
    GLuint gFragmentShaderObjectPV;
    GLuint gShaderProgramObjectPV;

    GLint modelMatrixUniformPV;
    GLint perspectiveProjectionUniformPV;
    GLint viewMatrixUniformPV;
    GLint lKeyPressedUniformPV; 
    mat4 perspectiveProjectionMatrix;
    

    GLint LightaUniformPV[2];
    GLint LightsUniformPV[2];
    GLint LightdUniformPV[2];
    GLint LightpUniformPV[2];

    GLint KaUniformPV; 
    GLint KdUniformPV;
    GLint KsUniformPV;
    GLint MaterialShineUniformPV;


    GLfloat rotation_Y_Axis;
    
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
        
        rotation_Y_Axis = 0.0f;
        bAnimate = NO;
        bLight = NO;
        
        
        gVertexShaderObjectPV = glCreateShader(GL_VERTEX_SHADER);

        const GLchar* vertexShaderSourceCodePV =
        "#version 300 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform int u_lKeyPressed;" \
        "uniform vec3 u_lightd[2];" \
        "uniform vec3 u_lighta[2];" \
        "uniform vec3 u_lights[2];" \
        "uniform vec4 u_lightp[2];" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ks;" \
        "uniform float materialShineUniform;" \
        "out vec3 phong_ads_light;" \
        "void main(void)" \
        "{" \
        "if(u_lKeyPressed == 1){" \
        "phong_ads_light = vec3(0.0f,0.0f,0.0f);"
        "vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
        "vec3 transformed_Normal = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
        "vec3 view_vector = normalize(-eyeCoordinate.xyz);" \
        "for (int i = 0; i < 2; i++) { " \
        "vec3 lightDirection = normalize(vec3(u_lightp[i] - eyeCoordinate)); "\
        "vec3 reflection_vector = reflect(-lightDirection, transformed_Normal);" \
        "vec3 ambient = u_lighta[i] * u_ka;" \
        "vec3 diffuse = u_lightd[i] * u_kd * max(dot(lightDirection, transformed_Normal) , 0.0f);" \
        "vec3 specular = u_lights[i] * u_ks * pow(max(dot(reflection_vector, view_vector), 0.0f) ,  materialShineUniform);" \
        "phong_ads_light = phong_ads_light + ambient + diffuse + specular;" \
        "}" \
        "}" \
        "else{" \
        "phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
        "}";
        
        glShaderSource(gVertexShaderObjectPV, 1, (const GLchar**)&vertexShaderSourceCodePV, NULL);

        glCompileShader(gVertexShaderObjectPV);
        
        GLint infoLogLength = 0;
        GLint shaderCompileStatus = 0;
        char* szBuffer = NULL;

        glGetShaderiv(gVertexShaderObjectPV, GL_COMPILE_STATUS, &shaderCompileStatus);
        if (shaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gVertexShaderObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
            szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
                if (szBuffer != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObjectPV, infoLogLength, &written, szBuffer);

                    printf("Vertex Shader Per Vertex Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }

        printf("Vertex Shader Done\n");
        
    gFragmentShaderObjectPV = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSourceCodePV =
        "#version 300 core" \
        "\n" \
        "in highp vec3 phong_ads_light;" \
        "out highp vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(phong_ads_light, 1.0);" \
        "}";

    glShaderSource(gFragmentShaderObjectPV, 1, (const GLchar**)&fragmentShaderSourceCodePV, NULL);


    glCompileShader(gFragmentShaderObjectPV);

    infoLogLength = 0;
    shaderCompileStatus = 0;
    szBuffer = NULL;

    glGetShaderiv(gFragmentShaderObjectPV, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus == GL_FALSE) {
        glGetShaderiv(gFragmentShaderObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
            if (szBuffer != NULL) {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObjectPV, infoLogLength, &written, szBuffer);

                printf("Fragment Shader Per Vertex Compilation Log : %s\n", szBuffer);
                free(szBuffer);
                [self release];
            }
        }
    }
    gShaderProgramObjectPV = glCreateProgram();
    glAttachShader(gShaderProgramObjectPV, gVertexShaderObjectPV);
    glAttachShader(gShaderProgramObjectPV, gFragmentShaderObjectPV);
    glBindAttribLocation(gShaderProgramObjectPV, SSK_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObjectPV, SSK_ATTRIBUTE_NORMAL, "vNormal");

    glLinkProgram(gShaderProgramObjectPV);

    infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    szBuffer = NULL;
    glGetProgramiv(gShaderProgramObjectPV, GL_LINK_STATUS, &shaderProgramLinkStatus);
    if (shaderProgramLinkStatus == GL_FALSE) {
        glGetProgramiv(gShaderProgramObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
            if (szBuffer != NULL) {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObjectPV, infoLogLength, &written, szBuffer);

                printf("Shader Program Log : %s\n", szBuffer);
                free(szBuffer);
                [self release];
            }
        }
    }

    modelMatrixUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_modelMatrix");
    viewMatrixUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_viewMatrix");
    perspectiveProjectionUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_projectionMatrix");
    lKeyPressedUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_lKeyPressed");
    LightdUniformPV[0] = glGetUniformLocation(gShaderProgramObjectPV, "u_lightd[0]");
    LightaUniformPV[0] = glGetUniformLocation(gShaderProgramObjectPV, "u_lighta[0]");
    LightsUniformPV[0] = glGetUniformLocation(gShaderProgramObjectPV, "u_lights[0]");
    LightpUniformPV[0] = glGetUniformLocation(gShaderProgramObjectPV, "u_lightp[0]");
    LightdUniformPV[1] = glGetUniformLocation(gShaderProgramObjectPV, "u_lightd[1]");
    LightaUniformPV[1] = glGetUniformLocation(gShaderProgramObjectPV, "u_lighta[1]");
    LightsUniformPV[1] = glGetUniformLocation(gShaderProgramObjectPV, "u_lights[1]");
    LightpUniformPV[1] = glGetUniformLocation(gShaderProgramObjectPV, "u_lightp[1]");
    KdUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_kd");
    KaUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_ka");
    KsUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_ks");
    MaterialShineUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "materialShineUniform");

    const GLfloat pyramidVertices[] =
    {
         0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         
         0.0f,  1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         
         0.0f,  1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         
         0.0f,  1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f
    };

    const GLfloat pyramidNormals[] = {
        
        0.0f, 0.447214f, 0.894427f,
        0.0f, 0.447214f, 0.894427f,
        0.0f, 0.447214f, 0.894427f,
        
        
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        
        
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f
    };

    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);

    glGenBuffers(1, &vbo_pyramid_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_pyramid_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.4f,0.4f,0.4f,1.0f);

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

-(void)drawView:(id)sender {
    // code
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLfloat lightAmbient0[] = { 0.0f,0.0f,0.0f };
    GLfloat lightDiffuse0[] = { 1.0f,0.0f,0.0f };
    GLfloat lightSpecular0[] = { 1.0f,0.0f,0.0f };
    GLfloat lightPosition0[] = { 2.0f,0.0f,0.0f,1.0f };

    GLfloat lightAmbient1[] = { 0.0f,0.0f,0.0f };
    GLfloat lightDiffuse1[] = { 0.0f,0.0f,1.0f };
    GLfloat lightSpecular1[] = { 0.0f,0.0f,1.0f };
    GLfloat lightPosition1[] = { -2.0f,0.0f,0.0f,1.0f };

    GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f };
    GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f };
    GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f };
    GLfloat materialShine = 50.0f;

    mat4 translateMatrix = mat4::identity();
    mat4 rotationMatrixY = mat4::identity();

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translateMatrix = vmath::translate(0.0f, 0.0f, -4.5f);
    rotationMatrixY = vmath::rotate(rotation_Y_Axis, 0.0f, 1.0f, 0.0f);

    modelMatrix = translateMatrix * rotationMatrixY;

    // order is important hence -> perspectiveProjectMatrix comes first
    // Per Vertex Model
    glUseProgram(gShaderProgramObjectPV);

    if(bLight == YES){
        glUniform1i(lKeyPressedUniformPV, 1);

        glUniform3fv(LightdUniformPV[0], 1, lightDiffuse0);
        glUniform3fv(LightaUniformPV[0], 1, lightAmbient0);
        glUniform3fv(LightsUniformPV[0], 1, lightSpecular0);
        glUniform4fv(LightpUniformPV[0], 1, lightPosition0); // Here 4fv needed all else can be if needed 3fv

        glUniform3fv(LightdUniformPV[1], 1, lightDiffuse1);
        glUniform3fv(LightaUniformPV[1], 1, lightAmbient1);
        glUniform3fv(LightsUniformPV[1], 1, lightSpecular1);
        glUniform4fv(LightpUniformPV[1], 1, lightPosition1); // Here 4fv needed all else can be if needed 3fv

        glUniform3fv(KdUniformPV, 1, materialDiffuse);
        glUniform3fv(KaUniformPV, 1, materialAmbient);
        glUniform3fv(KsUniformPV, 1, materialSpecular);
        glUniform1f(MaterialShineUniformPV, materialShine);
    }
    else{
        glUniform1i(lKeyPressedUniformPV, 0);
    }

    glUniformMatrix4fv(modelMatrixUniformPV, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniformPV, 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(viewMatrixUniformPV, 1, GL_FALSE, viewMatrix);

    glBindVertexArray(vao_pyramid);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);

    if (bAnimate == YES) {
        rotation_Y_Axis = rotation_Y_Axis + 0.5f;
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
