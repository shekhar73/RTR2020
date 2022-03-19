#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"
#import "GLESView.h"
#import "Sphere.h"
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


    GLuint gVao_sphere;
    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_element;

    GLuint gNumVertices;
    GLuint gNumElements;



    GLfloat factor = 10.0f;

    GLfloat angleX = 0.0f;
    GLfloat angleY = 0.0f;
    GLfloat angleZ = 0.0f;

    GLfloat lightPosition_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightAmbient_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightDiffuse_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightSpecular_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };

    GLfloat lightPosition_Blue[] = { 0.0f,0.0f,0.0f,1.0f };
    GLfloat lightAmbient_Blue[] = { 0.0f,0.0f,0.0f,1.0f };
    GLfloat lightDiffuse_Blue[] = { 0.0f,0.0f,1.0f,1.0f };
    GLfloat lightSpecular_Blue[] = { 0.0f,0.0f,1.0f,1.0f };

    GLfloat lightPosition_Green[] = { 0.0f,0.0f,0.0f,1.0f };
    GLfloat lightAmbient_Green[] = { 0.0f,0.0f,0.0f,1.0f };
    GLfloat lightDiffuse_Green[] = { 0.0f,1.0f,0.0f,1.0f };
    GLfloat lightSpecular_Green[] = { 0.0f,1.0f,0.0f,1.0f };

    GLfloat MaterialAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
    GLfloat MaterialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat MaterialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };

    GLfloat MaterialShininess = 128.0f;


    GLuint gModelMatrixUniform;
    GLuint gViewMatrixUniform;
    GLuint gPerspectiveProjectionMatrixUniform;

    // Light
    GLuint LaUniform_Red;
    GLuint LdUniform_Red;
    GLuint LsUniform_Red;
    GLuint lightPositionUniform_Red;

    GLuint LaUniform_Blue;
    GLuint LdUniform_Blue;
    GLuint LsUniform_Blue;
    GLuint lightPositionUniform_Blue;

    GLuint LaUniform_Green;
    GLuint LdUniform_Green;
    GLuint LsUniform_Green;
    GLuint lightPositionUniform_Green;

    // Material
    GLuint KaUniform;
    GLuint KdUniform;
    GLuint KsUniform;
    GLuint KshineUniform;

    GLuint LKeyPressedUniform;

    // It is a matrix of 4 X 4 which is declared in vmath.h header file. It will be used for transformation
    mat4 gPerspectiveProjectionMatrix;

    bool bLight;
    
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
        

       ///////////////////////////////////////////////////////////////////////////////////////
        //*** VERTEX SHADER ***
        //////////////////////////////////////////////////////////////////////////////////////

        // Create Shader
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

        // Provide source code to shader
        const GLchar* vertexShaderSourceCode =
            "#version 450 core" \
            "\n" \
                "in vec4 vPosition;" \
                "in vec3 vNormal;" \
                "uniform mat4 u_model_matrix;" \
                "uniform mat4 u_view_matrix;" \
                "uniform mat4 u_perspective_projection_matrix;" \
                "uniform vec3 u_la_red;" \
                "uniform vec3 u_ld_red;" \
                "uniform vec3 u_ls_red;" \
                "uniform vec4 u_light_position_red;" \
                "uniform vec3 u_la_blue;" \
                "uniform vec3 u_ld_blue;" \
                "uniform vec3 u_ls_blue;" \
                "uniform vec4 u_light_position_blue;" \
                "uniform vec3 u_la_green;" \
                "uniform vec3 u_ld_green;" \
                "uniform vec3 u_ls_green;" \
                "uniform vec4 u_light_position_green;" \
                "uniform vec3 u_ka;" \
                "uniform vec3 u_kd;" \
                "uniform vec3 u_ks;" \
                "uniform int u_LKeyPressed;" \
                "uniform float u_shininess;" \
                "out vec3 phong_ads_light;" \
                "void main(void)" \
                "{" \
                "if(u_LKeyPressed == 1)" \
                "{" \
                "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
                "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
                "vec3 view_vector = normalize(-eye_coordinates.xyz);" \

                "vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));" \
                "vec3 reflection_vector_red = reflect(-light_direction_red,transformed_normal);" \
                "vec3 ambient_red = u_la_red * u_ka;" \
                "vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red,transformed_normal),0.0f);" \
                "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,view_vector),0.0f),u_shininess);" \

                "vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" \
                "vec3 reflection_vector_blue = reflect(-light_direction_blue,transformed_normal);" \
                "vec3 ambient_blue = u_la_blue * u_ka;" \
                "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue,transformed_normal),0.0f);" \
                "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,view_vector),0.0f),u_shininess);" \

                "vec3 light_direction_green = normalize(vec3(u_light_position_green - eye_coordinates));" \
                "vec3 reflection_vector_green = reflect(-light_direction_green,transformed_normal);" \
                "vec3 ambient_green = u_la_green * u_ka;" \
                "vec3 diffuse_green = u_ld_green * u_kd * max(dot(light_direction_green,transformed_normal),0.0f);" \
                "vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green,view_vector),0.0f),u_shininess);" \

                "phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + diffuse_green + specular_red + specular_blue + specular_green;" \
                "}" \
                "else" \
                "{" \
                "phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
                "}" \
                "gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
                "}";

        // eye_coordinates.xyz = swizling
        //gl_Position is inbuilt variable in shader
        // feed the source code to your shaderobject
        glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

        // compile shader
        glCompileShader(gVertexShaderObject);

        GLint iInfoLogLength = 0;
        GLint iShaderCompiledStatus = 0;
        char* szInfoLog = NULL;

        // shader compilation error checking

        //void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
        // glGetShaderiv — return a parameter from a shader object

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

                    // take log
                    glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
                    fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    free(szBuffer);
                    [self release];
                }
            }
        }


        ///////////////////////////////////////////////////////////////////////////////////////
        //*** FRAGMENT SHADER ***
        ///////////////////////////////////////////////////////////////////////////////////////

        // Create shader
        gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

        // provide source code to shader
        const GLchar* fragmentShaderSourceCode =
            "#version 450 core" \
            "\n" \
            "\n" \
            "in vec3 phong_ads_light;" \
            "out vec4 FragColor;" \
            "void main(void)" \
            "{" \
            "FragColor = vec4(phong_ads_light,1.0f);" \
            "}";


        glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

        // compile shader
        glCompileShader(gFragmentShaderObject);

        // fragment shader compilation error checking


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
                    fprintf(gpFile, "Fragment Shader Compilation Log %s\n", szInfoLog);
                    free(szInfoLog);
                    free(szBuffer);
                    [self release];
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////
        //*** SHADER PROGRAM ***
        /////////////////////////////////////////////////////////////////////////////////////

        // create
        gShaderProgramObject = glCreateProgram();

        // attach vertex shader to shader program
        glAttachShader(gShaderProgramObject, gVertexShaderObject);

        // attach fragment shader to shader program
        glAttachShader(gShaderProgramObject, gFragmentShaderObject);

        // pre-link binding of shader program object with vertex shader position attribute in your enum to catch the in and unifrom attributes
        glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

        // link shader
        glLinkProgram(gShaderProgramObject);

        GLint iShaderProgramLinkStatus = 0;

        // Reset values
        iInfoLogLength = 0;
        szInfoLog = NULL;

        // shader link error checking
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
                    free(szBuffer);
                    [self release];
                }
            }

        }

        // get MVP uniform location
        // post linking


        gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_model_matrix");
        gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_view_matrix");
        gPerspectiveProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject,"u_perspective_projection_matrix");
        LaUniform_Red = glGetUniformLocation(gShaderProgramObject,"u_la_red");
        LdUniform_Red = glGetUniformLocation(gShaderProgramObject,"u_ld_red");
        LsUniform_Red = glGetUniformLocation(gShaderProgramObject,"u_ls_red");
        lightPositionUniform_Red = glGetUniformLocation(gShaderProgramObject,"u_light_position_red");
        LaUniform_Blue = glGetUniformLocation(gShaderProgramObject,"u_la_blue");
        LdUniform_Blue = glGetUniformLocation(gShaderProgramObject,"u_ld_blue");
        LsUniform_Blue = glGetUniformLocation(gShaderProgramObject,"u_ls_blue");
        lightPositionUniform_Blue = glGetUniformLocation(gShaderProgramObject,"u_light_position_blue");
        LaUniform_Green = glGetUniformLocation(gShaderProgramObject,"u_la_green");
        LdUniform_Green = glGetUniformLocation(gShaderProgramObject,"u_ld_green");
        LsUniform_Green = glGetUniformLocation(gShaderProgramObject,"u_ls_green");
        lightPositionUniform_Green = glGetUniformLocation(gShaderProgramObject,"u_light_position_green");
        KaUniform = glGetUniformLocation(gShaderProgramObject,"u_ka");
        KdUniform = glGetUniformLocation(gShaderProgramObject,"u_kd");
        KsUniform = glGetUniformLocation(gShaderProgramObject,"u_ks");
        KshineUniform = glGetUniformLocation(gShaderProgramObject,"u_shininess");
        LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject,"u_LKeyPressed");



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

        glShadeModel(GL_SMOOTH);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   // blue

        // set PerspectiveProjectionMatrix to identity matrix
        gPerspectiveProjectionMatrix = mat4::identity();

        bLight = false;

            
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

    glUseProgram(gShaderProgramObject);

    if (bLight == true)
    {
         lightPosition_Red[0] = 0.0f;
        lightPosition_Red[1] = factor * sin(angleX);
        lightPosition_Red[2] = factor * cos(angleX);
        lightPosition_Red[3] = 1.0f;

        lightPosition_Green[0] = factor * sin(angleY);
        lightPosition_Green[1] = 0.0f;
        lightPosition_Green[2] = factor * cos(angleY);
        lightPosition_Green[3] = 1.0f;

        lightPosition_Blue[0] = factor * sin(angleZ);
        lightPosition_Blue[1] = factor * cos(angleZ);
        lightPosition_Blue[2] = 0.0f;
        lightPosition_Blue[3] = 1.0f;

        glUniform1i(LKeyPressedUniform,1);
        glUniform1f(KshineUniform,MaterialShininess);
        glUniform4fv(lightPositionUniform_Red,1,lightPosition_Red);
        glUniform3fv(LaUniform_Red,1,lightAmbient_Red);
        glUniform3fv(LdUniform_Red,1,lightDiffuse_Red);
        glUniform3fv(LsUniform_Red,1,lightSpecular_Red);
        glUniform4fv(lightPositionUniform_Blue,1,lightPosition_Blue);
        glUniform3fv(LaUniform_Blue,1,lightAmbient_Blue);
        glUniform3fv(LdUniform_Blue,1,lightDiffuse_Blue);
        glUniform3fv(LsUniform_Blue,1,lightSpecular_Blue);
        glUniform4fv(lightPositionUniform_Green,1,lightPosition_Green);
        glUniform3fv(LaUniform_Green,1,lightAmbient_Green);
        glUniform3fv(LdUniform_Green,1,lightDiffuse_Green);
        glUniform3fv(LsUniform_Green,1,lightSpecular_Green);
        glUniform3fv(KaUniform,1,MaterialAmbient);
        glUniform3fv(KdUniform,1,MaterialDiffuse);
        glUniform3fv(KsUniform,1,MaterialSpecular);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }
     
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();
    mat4 translateMatrix = vmath::translate(0.0f,0.0f,-3.0f);

    modelMatrix = translateMatrix;
    projectionMatrix = gPerspectiveProjectionMatrix;

    glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(gPerspectiveProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


    // *** bind vao ***
    // all the rendering data is recoreded in initialize() by using glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao_sphere);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);

    // Stop using OpenGL program object
    glUseProgram(0);

     angleX = angleX + 0.01f;
    if(angleX >= 360.0f)
    {
        angleX = 0.0f;
    }

    angleY = angleY + 0.01f;
    if(angleY >= 360.0f)
    {
        angleY = 0.0f;
    }

    angleZ = angleZ + 0.01f;
    if(angleZ >= 360.0f)
    {
        angleZ = 0.0f;
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
