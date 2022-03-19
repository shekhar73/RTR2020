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
    mat4 perspectiveProjectionMatrix;
    BOOL bToggleLight;

    GLuint gVertexShaderObjectPV;
    GLuint gFragmentShaderObjectPV;
    GLuint gShaderProgramObjectPV;

    GLuint vao; 
    GLuint vbo_position; 
    GLuint vbo_normal;
    GLuint vbo_element;

    GLuint modelMatrixUniformPV;
    GLuint perspectiveProjectionUniformPV;
    GLuint viewMatrixUniformPV;

    GLuint LaUniformPV; 
    GLuint LsUniformPV; 
    GLuint LdUniformPV; 
    GLuint LpUniformPV; 

    GLuint KaUniformPV;
    GLuint KdUniformPV; 
    GLuint KsUniformPV;
    GLfloat MaterialShineUniformPV;

    GLuint lKeyPressedUniformPV; 


    GLuint gShaderProgramObjectPF;
    GLuint gVertexShaderObjectPF;
    GLuint gFragmentShaderObjectPF;

    GLuint modelMatrixUniformPF;
    GLuint perspectiveProjectionUniformPF;
    GLuint viewMatrixUniformPF;

    GLuint LaUniformPF; 
    GLuint LsUniformPF; 
    GLuint LdUniformPF; 
    GLuint LpUniformPF; 

    GLuint KaUniformPF;
    GLuint KdUniformPF; 
    GLuint KsUniformPF; 
    GLfloat MaterialShineUniformPF;

    GLuint lKeyPressedUniformPF;
    
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
        

        bLight = YES;
        bToggleLight = NO;
        
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
            "uniform vec3 u_ld;" \
            "uniform vec3 u_la;" \
            "uniform vec3 u_ls;" \
            "uniform vec4 light_position;" \
            "uniform vec3 u_kd;" \
            "uniform vec3 u_ka;" \
            "uniform vec3 u_ks;" \
            "uniform float materialShineUniform;" \
            "out vec3 phong_ads_light;" \
            "void main(void)" \
            "{" \
            "if(u_lKeyPressed == 1){" \
            "vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
            "vec3 transformed_Normal = normalize(mat3(u_viewMatrix * u_modelMatrix) * vNormal);" \
            "vec3 light_direction = normalize(vec3(light_position - eyeCoordinate));" \
            "vec3 reflection_vector = reflect(-light_direction, transformed_Normal);" \
            "vec3 view_vector = normalize(-eyeCoordinate.xyz);" \
            "vec3 ambient = u_la * u_ka;" \
            "vec3 diffuse = u_ld * u_kd * max(dot(light_direction, transformed_Normal) , 0.0f);" \
            "vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, view_vector), 0.0f) ,  materialShineUniform);" \
            "phong_ads_light = ambient + diffuse + specular;" \
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

                    printf("Vertex Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }
        

        gFragmentShaderObjectPV = glCreateShader(GL_FRAGMENT_SHADER);
        
        const GLchar* fragmentShaderSourceCodePV =
            "#version 300 core" \
            "\n" \
            "precision highp float;"\
            "in vec3 phong_ads_light;" \
            "out vec4 FragColor;" \
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

                    printf("Fragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }
        
        gVertexShaderObjectPF = glCreateShader(GL_VERTEX_SHADER);
       
        const GLchar* vertexShaderSourceCodePF =
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
            "vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" \
            "transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" \
            "light_direction = vec3(light_position - eyeCoordinate);" \
            "view_vector = -eyeCoordinate.xyz;" \
            "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" \
            "}";
        
        glShaderSource(gVertexShaderObjectPF, 1, (const GLchar**)&vertexShaderSourceCodePF, NULL);

        glCompileShader(gVertexShaderObjectPF);
        infoLogLength = 0;
        shaderCompileStatus = 0;
        szBuffer = NULL;

        glGetShaderiv(gVertexShaderObjectPF, GL_COMPILE_STATUS, &shaderCompileStatus);
        if (shaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gVertexShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
                if (szBuffer != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObjectPF, infoLogLength, &written, szBuffer);

                    printf("Vertex Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }

        gFragmentShaderObjectPF = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar* fragmentShaderSourceCodePF =
            "#version 300 core" \
            "\n" \
            "precision highp float;"\
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
        glShaderSource(gFragmentShaderObjectPF, 1, (const GLchar**)&fragmentShaderSourceCodePF, NULL);

        glCompileShader(gFragmentShaderObjectPF);
        
        infoLogLength = 0;
        shaderCompileStatus = 0;
        szBuffer = NULL;
        glGetShaderiv(gFragmentShaderObjectPF, GL_COMPILE_STATUS, &shaderCompileStatus);
        if (shaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gFragmentShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
                if (szBuffer != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gFragmentShaderObjectPF, infoLogLength, &written, szBuffer);

                    printf("Fragment Shader Compilation Log : %s\n", szBuffer);
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


        
        gShaderProgramObjectPF = glCreateProgram();
        glAttachShader(gShaderProgramObjectPF, gVertexShaderObjectPF);
        glAttachShader(gShaderProgramObjectPF, gFragmentShaderObjectPF);
        glBindAttribLocation(gShaderProgramObjectPF, SSK_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(gShaderProgramObjectPF, SSK_ATTRIBUTE_NORMAL, "vNormal");

        glLinkProgram(gShaderProgramObjectPF);
        infoLogLength = 0;
        shaderProgramLinkStatus = 0;
        szBuffer = NULL;
        glGetProgramiv(gShaderProgramObjectPF, GL_LINK_STATUS, &shaderProgramLinkStatus);
        if (shaderProgramLinkStatus == GL_FALSE) {
            glGetProgramiv(gShaderProgramObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                szBuffer = (char*)malloc(sizeof(char) * infoLogLength);
                if (szBuffer != NULL) {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObjectPF, infoLogLength, &written, szBuffer);

                    printf("Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }


        modelMatrixUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_modelMatrix");
        viewMatrixUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_viewMatrix");
        perspectiveProjectionUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_projectionMatrix");
        lKeyPressedUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_lKeyPressed");

        LdUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_ld");
        LaUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_la");
        LsUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_ls");
        LpUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "light_position");

        KdUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_kd");
        KaUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_ka");
        KsUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "u_ks");
        MaterialShineUniformPV = glGetUniformLocation(gShaderProgramObjectPV, "materialShineUniform");


        modelMatrixUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_modelMatrix");
        viewMatrixUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_viewMatrix");
        perspectiveProjectionUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_projectionMatrix");
        lKeyPressedUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_lKeyPressed");

        LdUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ld");
        LaUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_la");
        LsUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ls");
        LpUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "light_position");

        KdUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_kd");
        KaUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ka");
        KsUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "u_ks");
        MaterialShineUniformPF = glGetUniformLocation(gShaderProgramObjectPF, "materialShineUniform");
            

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
   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLfloat lightAmbient[] = { 0.2f,0.2f,0.2f,1.0f };
    GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f }; 
    GLfloat lightPosition[] = { 120.0f,120.0f,100.0f,1.0f };
    GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };


    GLfloat materialAmbient[] = { 0.1f,0.1f,0.1f,1.0f };
    GLfloat materialDiffuse[] = { 0.5f,0.2f,0.7f,1.0f };
    GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
    GLfloat materialShine = 128.0f;

    

    mat4 translateMatrix = mat4::identity();
    mat4 rotationMatrixX = mat4::identity();
    mat4 rotationMatrixY = mat4::identity();
    mat4 rotationMatrixZ = mat4::identity();
    mat4 scaleMatrix = mat4::identity();

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translateMatrix = vmath::translate(0.0f, 0.0f, -2.5f);

    modelMatrix = translateMatrix * rotationMatrixY;

    if (bToggleLight == NO) {
            glUseProgram(gShaderProgramObjectPV);

            if(bLight == YES)
            {
                glUniform1i(lKeyPressedUniformPV, 1);

                glUniform3fv(LdUniformPV, 1, lightDiffuse);
                glUniform3fv(LaUniformPV, 1, lightAmbient);
                glUniform3fv(LsUniformPV, 1, lightSpecular);
                glUniform4fv(LpUniformPV, 1, lightPosition);

                glUniform3fv(KdUniformPV, 1, materialDiffuse);
                glUniform3fv(KaUniformPV, 1, materialAmbient);
                glUniform3fv(KsUniformPV, 1, materialSpecular);
                glUniform1f(MaterialShineUniformPV, materialShine);

                glUniformMatrix4fv(modelMatrixUniformPV, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(perspectiveProjectionUniformPV, 1, GL_FALSE, perspectiveProjectionMatrix);
                glUniformMatrix4fv(viewMatrixUniformPV, 1, GL_FALSE, viewMatrix);
            }
            else
            {
                glUniform1i(lKeyPressedUniformPV, 0);
            }
        
    }
    else 
    {
            glUseProgram(gShaderProgramObjectPF);

            if(bLight == YES)
            {
                glUniform1i(lKeyPressedUniformPF, 1);

                glUniform3fv(LdUniformPF, 1, lightDiffuse);
                glUniform3fv(LaUniformPF, 1, lightAmbient);
                glUniform3fv(LsUniformPF, 1, lightSpecular);
                glUniform4fv(LpUniformPF, 1, lightPosition);

                glUniform3fv(KdUniformPF, 1, materialDiffuse);
                glUniform3fv(KaUniformPF, 1, materialAmbient);
                glUniform3fv(KsUniformPF, 1, materialSpecular);
                glUniform1f(MaterialShineUniformPF, materialShine);

                glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(perspectiveProjectionUniformPF, 1, GL_FALSE, perspectiveProjectionMatrix);
                glUniformMatrix4fv(viewMatrixUniformPF, 1, GL_FALSE, viewMatrix);
            }
            else
            {
                glUniform1i(lKeyPressedUniformPF, 0);
            }
    }
    

    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
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
