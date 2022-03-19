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

    BOOL bLight;

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

    GLuint LaUniform; 
    GLuint LsUniform; 
    GLuint LdUniform;
    GLuint LpUniform; 

    GLuint KaUniform; 
    GLuint KdUniform; 
    GLuint KsUniform; 
    GLfloat MaterialShineUniform;

    GLuint lKeyPressedUniform; 

    mat4 perspectiveProjectionMatrix;
    
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
        

        bLight = NO;
        
        
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

        const GLchar* vertexShaderSourceCode =
        "#version 300 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform int u_lKeyPressed;" \
        "uniform vec4 light_position;" \
        "out vec3 transformed_Normal;" \
        "out vec3 light_direction;" \
        "out vec3 view_vector;" \
        "void main(void)" \
        "{" \
        "if(u_lKeyPressed == 1){" \
        "vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
        "transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
        "light_direction = vec3(light_position - eyeCoordinate);" \
        "view_vector = -eyeCoordinate.xyz;" \
        "}" \
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
        "precision highp float;" \
        "vec3 phong_ads_light;" \
        "in vec3 transformed_Normal;" \
        "in vec3 light_direction;" \
        "in vec3 view_vector;" \
        "out vec4 FragColor;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ls;" \
        "uniform vec4 light_position;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ks;" \
        "uniform float materialShineUniform;" \
        "uniform highp int u_lKeyPressed;" \
        "void main(void)" \
        "{" \
        "if(u_lKeyPressed == 1){" \
        "vec3 normalizedTransformNormal = normalize(transformed_Normal);" \
        "vec3 normalizedLightDirection = normalize(light_direction);" \
        "vec3 normalizedViewVector = normalize(view_vector);" \
        "vec3 ambient = u_la * u_ka;" \
        "vec3 diffuse = u_ld * u_kd * max(dot(normalizedLightDirection, normalizedTransformNormal) , 0.0f);" \
        "vec3 reflection_vector = reflect(-normalizedLightDirection, normalizedTransformNormal);" \
        "vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalizedViewVector), 0.0f) ,  materialShineUniform);" \
        "phong_ads_light = ambient + diffuse + specular;" \
        "}" \
        "else{" \
        "phong_ads_light = vec3(1.0,1.0,1.0f);" \
        "}" \
        "FragColor = vec4(phong_ads_light, 1.0);" \
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

        glGenBuffers(1, &vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
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

    GLfloat lightAmbient[] = { 0.2f,0.2f,0.2f,1.0f };
    GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat lightPosition[] = { 120.0f,120.0f,100.0f,1.0f };
    GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };

    GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
    GLfloat materialDiffuse[] = { 0.5f,0.2f,0.7f,1.0f };
    GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
    GLfloat materialShine = 128.0f;

    if (bLight == YES) 
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

    mat4 translateMatrix = mat4::identity();
    mat4 rotationMatrixX = mat4::identity();
    mat4 rotationMatrixY = mat4::identity();
    mat4 rotationMatrixZ = mat4::identity();
    mat4 scaleMatrix = mat4::identity();

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    modelMatrix = translateMatrix * rotationMatrixY;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);

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
