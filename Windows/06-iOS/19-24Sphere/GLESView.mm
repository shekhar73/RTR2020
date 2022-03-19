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

    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4]; 
    GLfloat lightPosition[4]; 

    GLuint LaUniform; 
    GLuint LsUniform; 
    GLuint LdUniform;
    GLuint LpUniform; 

    GLuint KaUniform; 
    GLuint KdUniform; 
    GLuint KsUniform; 
    GLfloat MaterialShineUniform;

    GLuint lKeyPressedUniform; 

    GLfloat lightRotateAngleX;
    
    GLint width;
    GLint height;

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
        

        lightAmbient[0] = 0.0f;
        lightAmbient[1] = 0.0f;
        lightAmbient[2] = 0.0f;
        lightAmbient[3] = 1.0f;

        for(int i=0; i<4; i++)
        {
            lightDiffuse[i] = 1.0f;
            lightSpecular[i] = 1.0f;
            lightPosition[i] = 0.0f;
        }

        lightRotateAngleX = 0.0f;
        
        bAnimateXRotation = 1;
        
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
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
       
        glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCodePF, NULL);

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

                    // Per Fragment Log
                    printf("Vertex Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                }
            }
        }

        gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
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
        glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCodePF, NULL);

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

                    printf("Shader Compilation Log : %s\n", szBuffer);
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

    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShine;

    mat4 translateMatrix = mat4::identity();
    mat4 rotationMatrixX = mat4::identity();
    mat4 rotationMatrixY = mat4::identity();
    mat4 rotationMatrixZ = mat4::identity();
    mat4 scaleMatrix = mat4::identity();

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    glUseProgram(gShaderProgramObject);

    if (bLight == YES) {
        
        glUniform1i(lKeyPressedUniform, 1);

        glUniform3fv(LdUniform, 1, lightDiffuse);
        glUniform3fv(LaUniform, 1, lightAmbient);
        glUniform3fv(LsUniform, 1, lightSpecular);
        glUniform4fv(LpUniform, 1, lightPosition); 

    }
    else 
    {
    
        glUniform1i(lKeyPressedUniform, 0);
    }

    glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);


    glViewport(0, (GLsizei)(height/6) * 4, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0215f;
    materialAmbient[1] = 0.1745f;
    materialAmbient[2] = 0.0215f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.07568f;
    materialDiffuse[1] = 0.61424f;
    materialDiffuse[2] = 0.07568f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.633f;
    materialSpecular[1] = 0.727811f;
    materialSpecular[2] = 0.633f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.6f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6), (GLsizei)(height/6) * 4, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    materialAmbient[0] = 0.135f;
    materialAmbient[1] = 0.2225f;
    materialAmbient[2] = 0.1575f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.54f;
    materialDiffuse[1] = 0.89f;
    materialDiffuse[2] = 0.63f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.316228f;
    materialSpecular[1] = 0.316228f;
    materialSpecular[2] = 0.316228f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.1f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 2, (GLsizei)(height/6) * 4, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    
 
    
    materialAmbient[0] = 0.05375f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.06625f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.18275f;
    materialDiffuse[1] = 0.17f;
    materialDiffuse[2] = 0.22525f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.332741f;
    materialSpecular[1] = 0.328634f;
    materialSpecular[2] = 0.346435f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.3f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);



    glViewport((GLsizei)(width/6) * 3, (GLsizei)(height/6) * 4, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.25f;
    materialAmbient[1] = 0.20725f;
    materialAmbient[2] = 0.20725f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 0.829f;
    materialDiffuse[2] = 0.829f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.296648f;
    materialSpecular[1] = 0.296648f;
    materialSpecular[2] = 0.296648f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.088f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 4, (GLsizei)(height/6) * 4, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    materialAmbient[0] = 0.1745f;
    materialAmbient[1] = 0.01175f;
    materialAmbient[2] = 0.01175f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.61424f;
    materialDiffuse[1] = 0.04136f;
    materialDiffuse[2] = 0.04136f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.727811f;
    materialSpecular[1] = 0.626959f;
    materialSpecular[2] = 0.626959f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.6f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 5, (GLsizei)(height/6) * 4, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.1f;
    materialAmbient[1] = 0.18725f;
    materialAmbient[2] = 0.1745f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.396f;
    materialDiffuse[1] = 0.74151f;
    materialDiffuse[2] = 0.69102f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.297254f;
    materialSpecular[1] = 0.30829f;
    materialSpecular[2] = 0.306678f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.1f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glViewport(0, (GLsizei)(height/6) * 3, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.329412f;
    materialAmbient[1] = 0.223529f;
    materialAmbient[2] = 0.027451f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.780392f;
    materialDiffuse[1] = 0.568627f;
    materialDiffuse[2] = 0.113725f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.992157f;
    materialSpecular[1] = 0.941176f;
    materialSpecular[2] = 0.807843f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.21794872f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6), (GLsizei)(height/6) * 3, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    
  
    
    materialAmbient[0] = 0.2125f;
    materialAmbient[1] = 0.1275f;
    materialAmbient[2] = 0.054f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.714f;
    materialDiffuse[1] = 0.4284f;
    materialDiffuse[2] = 0.18144f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.393548f;
    materialSpecular[1] = 0.271906f;
    materialSpecular[2] = 0.166721f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.2f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 2, (GLsizei)(height/6) * 3, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.25f;
    materialAmbient[1] = 0.25f;
    materialAmbient[2] = 0.25f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.4f;
    materialDiffuse[1] = 0.4f;
    materialDiffuse[2] = 0.4f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.774597f;
    materialSpecular[1] = 0.774597f;
    materialSpecular[2] = 0.774597f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.6f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 3, (GLsizei)(height/6) * 3, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    
    
    materialAmbient[0] = 0.19125f;
    materialAmbient[1] = 0.0735f;
    materialAmbient[2] = 0.0225f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.7038f;
    materialDiffuse[1] = 0.27048f;
    materialDiffuse[2] = 0.0828f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.256777f;
    materialSpecular[1] = 0.137622f;
    materialSpecular[2] = 0.086014f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.1f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 4, (GLsizei)(height/6) * 3, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    
    materialAmbient[0] = 0.24725f;
    materialAmbient[1] = 0.1995f;
    materialAmbient[2] = 0.0745f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.75164f;
    materialDiffuse[1] = 0.60648f;
    materialDiffuse[2] = 0.22648f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.628281f;
    materialSpecular[1] = 0.555802f;
    materialSpecular[2] = 0.366065f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.4f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 5, (GLsizei)(height/6) * 3, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.19225f;
    materialAmbient[1] = 0.19225f;
    materialAmbient[2] = 0.19225f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.50754f;
    materialDiffuse[1] = 0.50754f;
    materialDiffuse[2] = 0.50754f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.508273f;
    materialSpecular[1] = 0.508273f;
    materialSpecular[2] = 0.508273f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.4f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport(0, (GLsizei)(height/6) * 2, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.01f;
    materialDiffuse[1] = 0.01f;
    materialDiffuse[2] = 0.01f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.50f;
    materialSpecular[1] = 0.50f;
    materialSpecular[2] = 0.50f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.25f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6), (GLsizei)(height/6) * 2, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.1f;
    materialAmbient[2] = 0.06f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.0f;
    materialDiffuse[1] = 0.50980392f;
    materialDiffuse[2] = 0.50980392f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.50196078f;
    materialSpecular[1] = 0.50196078f;
    materialSpecular[2] = 0.50196078f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.25f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 2, (GLsizei)(height/6) * 2, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.1f;
    materialDiffuse[1] = 0.35f;
    materialDiffuse[2] = 0.1f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.45f;
    materialSpecular[1] = 0.55f;
    materialSpecular[2] = 0.45f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.25f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);



    glViewport((GLsizei)(width/6) * 3, (GLsizei)(height/6) * 2, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    
   
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.0f;
    materialDiffuse[2] = 0.0f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.6f;
    materialSpecular[2] = 0.6f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.25f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 4, (GLsizei)(height/6) * 2, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.55f;
    materialDiffuse[1] = 0.55f;
    materialDiffuse[2] = 0.55f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.70f;
    materialSpecular[1] = 0.70f;
    materialSpecular[2] = 0.70f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.25f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 5, (GLsizei)(height/6) * 2, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.5f;
    materialDiffuse[2] = 0.0f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.60f;
    materialSpecular[1] = 0.60f;
    materialSpecular[2] = 0.50f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.25f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);



    glViewport(0, (GLsizei)(height/6) * 1, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.02f;
    materialAmbient[1] = 0.02f;
    materialAmbient[2] = 0.02f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.01f;
    materialDiffuse[1] = 0.01f;
    materialDiffuse[2] = 0.01f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.4f;
    materialSpecular[1] = 0.4f;
    materialSpecular[2] = 0.4f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.078125f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6), (GLsizei)(height/6) * 1, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.05f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.4f;
    materialDiffuse[1] = 0.5f;
    materialDiffuse[2] = 0.5f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.04f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.078125f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 2, (GLsizei)(height/6) * 1, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.4f;
    materialDiffuse[1] = 0.5f;
    materialDiffuse[2] = 0.4f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.04f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.04f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.078125f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);



    glViewport((GLsizei)(width/6) * 3, (GLsizei)(height/6) * 1, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    
    materialAmbient[0] = 0.05f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.4f;
    materialDiffuse[2] = 0.4f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.04f;
    materialSpecular[2] = 0.04f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.078125f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 4, (GLsizei)(height/6) * 1, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    

    materialAmbient[0] = 0.05f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.05f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.5f;
    materialDiffuse[2] = 0.5f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.078125f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);


    glViewport((GLsizei)(width/6) * 5, (GLsizei)(height/6) * 1, (GLsizei)(width/6), (GLsizei)(height/6));
    translateMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translateMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    
    //------------------------------------------------
 
    
    materialAmbient[0] = 0.05f;
    materialAmbient[1] = 0.05f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
   
    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.5f;
    materialDiffuse[2] = 0.4f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.04f;
    materialSpecular[3] = 1.0f;
    
    materialShine = 0.078125f * 128.0f;
    glUniform3fv(KdUniform, 1, materialDiffuse);
    glUniform3fv(KaUniform, 1, materialAmbient);
    glUniform3fv(KsUniform, 1, materialSpecular);
    glUniform1f(MaterialShineUniform, materialShine );
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
    glDrawElements(GL_TRIANGLES, gNumSphereElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    // stop using OpenGL program object
    glUseProgram(0);
    

    if (bAnimateXRotation == 1) {
        lightPosition[0] = 0.0f;
        lightPosition[1] = 7.0f * sin(lightRotateAngleX);
        lightPosition[2] = 7.0f * cos(lightRotateAngleX);
        lightPosition[3] = 0.0f;

        lightRotateAngleX = lightRotateAngleX + 0.01f;
    }
    else if (bAnimateXRotation == 2) {
        lightPosition[0] = 7.0f * sin(lightRotateAngleX);
        lightPosition[1] = 0.0f;
        lightPosition[2] = 7.0f * cos(lightRotateAngleX);
        lightPosition[3] = 0.0f;

        lightRotateAngleX = lightRotateAngleX + 0.01f;
    }
    else if (bAnimateXRotation == 3){
        lightPosition[0] = 7.0f * sin(lightRotateAngleX);
        lightPosition[1] = 7.0f * cos(lightRotateAngleX);
        lightPosition[2] = 0.0f;
        lightPosition[3] = 0.0f;

        lightRotateAngleX = lightRotateAngleX + 0.01f;
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
    if(isAnimating == NO){
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        isAnimating = YES;
    }
}

-(void)stopAnimation {
    if(isAnimating == YES){
        [displayLink invalidate];
        displayLink = nil;
        
        isAnimating = NO;
    }
}
    

/* Event Listener Methods our Own */
-(void)onSingleTap:(UITapGestureRecognizer *)gr{
    // Code
    if (bLight == YES) {
        bLight = NO;
    }
    else {
        bLight = YES;
    }
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr{
    // Code
    if(bAnimateXRotation > 3){
        bAnimateXRotation = 0;
    }
    else{
        bAnimateXRotation++;
    }
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr{
    [self release];
    exit(0);
}


-(BOOL)acceptsFirstResponder {
    // code
    return (YES);
}

-(void)touchesBegan:(NSSet *)touches withEvent: (UIEvent *)event {
    // code

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
