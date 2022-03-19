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

    GLuint vao_graph;
    GLuint vbo_graph_vertex; 
    GLuint vbo_graph_color;

   
    GLuint vao_axis;
    GLuint vbo_axis_vertex;
    GLuint vbo_axis_color;


   
    GLuint vao_rect;
    GLuint vbo_rect_vertex;
    GLuint vbo_rect_color;

   
    GLuint vao_circle_1;
    GLuint vbo_circle_1_vertex;
    GLuint vbo_circle1_color;

   
    GLuint vao_triangle;
    GLuint vbo_triangle_vertex;
    GLuint vbo_triangle_color;

   
    GLuint vao_circle_2;
    GLuint vbo_circle_2_vertex;
    GLuint vbo_circle2_color;

    GLuint mvpMatrixUniform;
    mat4 perspectiveProjectMatrix;

    GLfloat InCenterX;
    GLfloat InCenterY;
    
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
        
        isAnimating = NO;
        animationFrameInterval = 60;



         gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* vertexShaderSourceCode =
            "#version 300 core" \
            "\n" \
            "in vec4 vPosition;" \
            "in vec4 vColor;" \
            "uniform mat4 u_mvpMatrix;" \
            "out vec4 out_color;" \
            "void main(void)" \
            "{" \
            "gl_Position = u_mvpMatrix * vPosition;" \
            "out_color = vColor;" \
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
            "in highp vec4 out_color;"
            "out highp vec4 FragColor;" \
            "void main(void)" \
            "{" \
            "FragColor = out_color;" \
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
        glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_COLOR, "vColor");
        

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

        mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

        GLfloat graphVertices[42 * 2 * 3 * 2];

        GLfloat x = -1.0f;
        for(int i=0; i<41*2*3; i=i+6){
            graphVertices[i]     = x;
            graphVertices[i+1]     = 1.0f;
            graphVertices[i+2]    = 0.0f;

            graphVertices[i+3]     = x;
            graphVertices[i+4]     = -1.0f;
            graphVertices[i+5]    = 0.0f;

            x = x + 0.05f;
        }

        // X Lines
        GLfloat y = 1.0f;
        for(int i=41*2*3; i<41*2*3*2; i=i+6){
            graphVertices[i]     = -1.0f;
            graphVertices[i+1]     = y;
            graphVertices[i+2]    = 0.0f;

            graphVertices[i+3]     = 1.0f;
            graphVertices[i+4]     = y;
            graphVertices[i+5]    = 0.0f;

            y = y - 0.05f;
        }


        GLfloat graphColors[42*2*3*2];
        for(int i=0; i<42*2*3*2; i=i+3){
            graphColors[i]         = 0.0f;
            graphColors[i+1]     = 0.0f;
            graphColors[i+2]     = 1.0f;
        }

        glGenVertexArrays(1, &vao_graph);
        glBindVertexArray(vao_graph);

        glGenBuffers(1, &vbo_graph_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graph_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphVertices), graphVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_graph_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graph_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphColors), graphColors, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        const GLfloat AxisVertices[] = {
            -1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, -1.0f, 0.0f
        };
        const GLfloat AxisColors[] = {
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };
        glGenVertexArrays(1, &vao_axis);
        glBindVertexArray(vao_axis);

        glGenBuffers(1, &vbo_axis_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_axis_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AxisVertices), AxisVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_axis_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_axis_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AxisColors), AxisColors, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        const GLfloat rectVertices[] = {
            -1.0f, 0.75f, 0.0f,
            -1.0f, -0.75f, 0.0f,

            -1.0f, -0.75f, 0.0f,
            1.0f, -0.75f, 0.0f,

            1.0f, -0.75f, 0.0f,
            1.0f, 0.75f, 0.0f,

            1.0f, 0.75f, 0.0f,
            -1.0f, 0.75f, 0.0f
        };

        const GLfloat rectColors[] = {
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };
        glGenVertexArrays(1, &vao_rect);
        glBindVertexArray(vao_rect);

        glGenBuffers(1, &vbo_rect_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_rect_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_rect_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_rect_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectColors), rectColors, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        GLfloat radius = 0.75f;
        GLfloat circle1Vertices[360*9];
        GLfloat circle1Colors[360*9];

        GLfloat angle = 0.0f;
        for(int i=0; i<360*9; i=i+3){
            circle1Vertices[i] = radius * cos(angle);
            circle1Vertices[i+1] = radius * sin(angle);
            circle1Vertices[i+2] = 0.0f;

            angle = angle + 0.006f;

            circle1Colors[i] = 1.0f;
            circle1Colors[i+1] = 0.05f;
            circle1Colors[i+2] = 1.0f;
        }
        
        glGenVertexArrays(1, &vao_circle_1);
        glBindVertexArray(vao_circle_1);

        glGenBuffers(1, &vbo_circle_1_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_1_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circle1Vertices), circle1Vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_circle1_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle1_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circle1Colors), circle1Colors, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        GLfloat x1 = 0.0f;
        GLfloat y1 = 0.75f;

        GLfloat x2 = (cos(120.0f) * 0.75f);
        GLfloat y2 = -(sin(120.0f) * 0.75f);

        GLfloat x3 = -(cos(120.0f) * 0.75f);
        GLfloat y3 = -(sin(120.0f) * 0.75f);
        const GLfloat triangleVertices[] = {
            x1, y1, 0.0f,
            x2, y2, 0.0f,

            x2, y2, 0.0f,
            x3, y3, 0.0f,

            x3, y3, 0.0f,
            x1, y1, 0.0f,
        };

        GLfloat triangleColors[] = {
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f
        };
        glGenVertexArrays(1, &vao_triangle);
        glBindVertexArray(vao_triangle);

        glGenBuffers(1, &vbo_triangle_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_triangle_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

 
        GLfloat a = sqrt(((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)));
        GLfloat b = sqrt(((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)));
        GLfloat c = sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
        InCenterX = ((a * x1) + (b * x2) + (c * x3))/(a+b+c);
        InCenterY = ((a * y1) + (b * y2) + (c * y3))/(a+b+c);
        GLfloat Perimeter = (a + b + c) / 2;
        GLfloat Area = sqrt(Perimeter * (Perimeter - a) * (Perimeter - b) * (Perimeter - c));
        GLfloat radiusInCircle = Area / Perimeter;

        GLfloat circle2Vertices[360*9];
        GLfloat circle2Colors[360*9];

        angle = 0.0f;
        for(int i=0; i<360*9; i=i+3){
            circle2Vertices[i] = radiusInCircle * cos(angle);
            circle2Vertices[i+1] = radiusInCircle * sin(angle);
            circle2Vertices[i+2] = 0.0f;

            angle = angle + 0.006f;

            circle2Colors[i] = 1.5f;
            circle2Colors[i+1] = 0.05f;
            circle2Colors[i+2] = 0.0f;
        }
        glGenVertexArrays(1, &vao_circle_2);
        glBindVertexArray(vao_circle_2);

        glGenBuffers(1, &vbo_circle_2_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_2_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circle2Vertices), circle2Vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_circle2_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle2_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circle2Colors), circle2Colors, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindVertexArray(0);

        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE); // Remove in 3D Animations - Here No Backface Rendering
        
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        
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
    
    // Start Using OpenGL program object
    glUseProgram(gShaderProgramObject);

    mat4 translateMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    // graph
    translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelViewMatrix = translateMatrix;
    modelViewProjectionMatrix = perspectiveProjectMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    
    glBindVertexArray(vao_graph);
    for(int i=0; i<41*2; i++){
        glDrawArrays(GL_LINES, i*2 , 2); // take input from i*2 index and take 3(vertices of point)*2
    }
    glBindVertexArray(0);

    glBindVertexArray(vao_axis);
    glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_LINES, 2, 2);
    glBindVertexArray(0);

    glBindVertexArray(vao_rect);
    for(int i=0; i<8; i++){
        glDrawArrays(GL_LINES, i*2 , 2);
    }
    glBindVertexArray(0);


    glBindVertexArray(vao_circle_1);
    for(int i=0; i<360*9; i++){
        glDrawArrays(GL_POINTS, i , 1);
    }
    glBindVertexArray(0);


    glBindVertexArray(vao_triangle);
    for(int i=0; i<6; i++){
        glDrawArrays(GL_LINES, i*2 , 2); //last parameter is set of 2
    }
    glBindVertexArray(0);


    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = vmath::translate(InCenterX, InCenterY, -3.0f);
    modelViewMatrix = translateMatrix;
    modelViewProjectionMatrix = perspectiveProjectMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glBindVertexArray(vao_circle_2);
    for(int i=0; i<360*9; i++){
        glDrawArrays(GL_POINTS, i , 1);
    }
    glBindVertexArray(0);

    // stop using OpenGL program object
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
