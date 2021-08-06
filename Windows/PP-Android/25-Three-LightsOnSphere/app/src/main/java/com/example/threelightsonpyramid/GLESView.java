package com.example.threelightsonpyramid;

import android.content.Context;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import android.opengl.GLES32;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import android.opengl.Matrix;

// A view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;

    int factor = 10;
    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;

	float lightPosition_Red_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightAmbient_Red_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse_Red_PV[] 	= {1.0f, 0.0f, 0.0f, 1.0f};
	float lightSpecular_Red_PV[] 	= {1.0f, 0.0f, 0.0f, 1.0f};

	float lightPosition_Blue_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightAmbient_Blue_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse_Blue_PV[] 	= {0.0f, 0.0f, 1.0f, 1.0f};
	float lightSpecular_Blue_PV[] 	= {0.0f, 0.0f, 1.0f, 1.0f};

	float lightPosition_Green_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightAmbient_Green_PV[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse_Green_PV[] 	= {0.0f, 1.0f, 0.0f, 1.0f};
	float lightSpecular_Green_PV[] 	= {0.0f, 1.0f, 0.0f, 1.0f};

	float MaterialAmbient_PV[] 		= {0.0f, 0.0f, 0.0f, 0.0f};
	float MaterialDiffuse_PV[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
	float MaterialSpecular_PV[] 	= {1.0f, 1.0f, 1.0f, 1.0f};
	float MaterialShininess_PV 		= 50.0f;

	float lightPosition_Red_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightAmbient_Red_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse_Red_PF[] 	= {1.0f, 0.0f, 0.0f, 1.0f};
	float lightSpecular_Red_PF[] 	= {1.0f, 0.0f, 0.0f, 1.0f};

	float lightPosition_Blue_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightAmbient_Blue_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse_Blue_PF[] 	= {0.0f, 0.0f, 1.0f, 1.0f};
	float lightSpecular_Blue_PF[] 	= {0.0f, 0.0f, 1.0f, 1.0f};

	float lightPosition_Green_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightAmbient_Green_PF[] 	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDiffuse_Green_PF[] 	= {0.0f, 1.0f, 0.0f, 1.0f};
	float lightSpecular_Green_PF[] 	= {0.0f, 1.0f, 0.0f, 1.0f};

	float MaterialAmbient_PF[] 		= {0.0f, 0.0f, 0.0f, 0.0f};
	float MaterialDiffuse_PF[] 		= {1.0f, 1.0f, 1.0f, 1.0f};
	float MaterialSpecular_PF[] 	= {1.0f, 1.0f, 1.0f, 1.0f};
	float MaterialShininess_PF 		= 128.0f;

	private int vertexShaderObject_PV;
	private int fragmentShaderObject_PV;
	private int shaderProgramObject_PV;

	private int vertexShaderObject_PF;
	private int fragmentShaderObject_PF;
	private int shaderProgramObject_PF;

	private int[] vao_sphere = new int[1];
	private int[] vbo_sphere_position = new int[1];
	private int[] vbo_sphere_normal = new int[1];
	private int[] vbo_sphere_element = new int[1];
    int numVertices;
    int numElements;

	private int modelMatrixUniform_PV;                             
	private int viewMatrixUniform_PV;
	private int perspectiveProjectionUniform_PV;

	private int LaUniform_Red_PV;
	private int LdUniform_Red_PV;
	private int LsUniform_Red_PV;
	private int lightPositionUniform_Red_PV;
    
	private int LaUniform_Blue_PV;
	private int LdUniform_Blue_PV;
	private int LsUniform_Blue_PV;
	private int lightPositionUniform_Blue_PV;

	private int LaUniform_Green_PV;
	private int LdUniform_Green_PV;
	private int LsUniform_Green_PV;
	private int lightPositionUniform_Green_PV;

	private int KaUniform_PV;
	private int KdUniform_PV;
	private int KsUniform_PV;
	private int KshineUniform_PV;
	private int LKeyPressedUniform_PV;

	private int modelMatrixUniform_PF;                             
	private int viewMatrixUniform_PF;
	private int perspectiveProjectionUniform_PF;

	private int LaUniform_Red_PF;
	private int LdUniform_Red_PF;
	private int LsUniform_Red_PF;
	private int lightPositionUniform_Red_PF;
    
	private int LaUniform_Blue_PF;
	private int LdUniform_Blue_PF;
	private int LsUniform_Blue_PF;
	private int lightPositionUniform_Blue_PF;

	private int LaUniform_Green_PF;
	private int LdUniform_Green_PF;
	private int LsUniform_Green_PF;
	private int lightPositionUniform_Green_PF;

	private int KaUniform_PF;
	private int KdUniform_PF;
	private int KsUniform_PF;
	private int KshineUniform_PF;
	private int LKeyPressedUniform_PF;


	public int tapOnScreen;
	private boolean bLight;

	private float perspectiveProjectionMatrix[] = new float[16];	// 4x4 matrix

	public GLESView(Context drawingContext)
	{
		super(drawingContext);

		context = drawingContext;

		// accordingly set EGLContext to current supported version of OpenGLES
		setEGLContextClientVersion(3);

		// set Renderer for drawing on the GLSurfaceView
		setRenderer(this);

		// Render the view only when there is a change in the drawing data
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); // JUST LIKE INVALIDATE RECT

		gestureDetector = new GestureDetector(context, this, null, false); // this means 'handler' i.e. who is going to handle
		gestureDetector.setOnDoubleTapListener(this);	// this means 'handler' i.e. who is going to handle
	} 

	// overriden method of GLSurfaceView.Renderer ( Init Code )
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		// OpenGL-ES version check
		String glesVersion = gl.glGetString(GL10.GL_VERSION);
		System.out.println("SSK: "+glesVersion);

		// get GLSL version
		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("SSK: GLSL Version = "+glslVersion);
		initialize(gl);
	}


	// overriden method of GLSurfaceView.Renderer ( Change Size Code )
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		resize(width, height);
	}

	// overriden method of GLSurfaceView.Renderer ( Rendering Code )
	@Override
	public void onDrawFrame(GL10 unused)
	{
		draw();
	}

	// Handling 'OnTouchEvent' Is The Most IMPORTANT, Because It Triggers All Gesture And Tap Events
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		// code
		int eventaction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return (true);

	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		if(tapOnScreen == 0)
			tapOnScreen = 1;
		else
			tapOnScreen = 0;
		System.out.println("SSK: "+"Double Tap");
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		System.out.println("SSK : Double Tap.");
		tapOnScreen = 1;
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		if(bLight == false)
			bLight = true;
		else
			bLight = false;
		System.out.println("SSK: "+"Single Tap");
		tapOnScreen = 0;
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onDown(MotionEvent e)
	{
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public void onLongPress(MotionEvent e)
	{
		System.out.println("SSK: "+"Long Press");
	}

	// abstrct method from OnGestureListner so must be implemented
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		System.out.println("SSK: "+"Scroll");
		//unInitialize();
		System.exit(0);
		return (true);
		
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public void onShowPress(MotionEvent e)
	{
	}

	// abstrct mehtod from OnGestrureListener so must be implemented
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return (true);
	}

	private void initialize(GL10 gl)
	{
		//**********************************************
		// Vertex Shader
		//**********************************************
		// Create Shader
		//*********************************************
		vertexShaderObject_PV = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		// vertex shader source code
		final String vertexShaderSourceCode_PV = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec3 vNormal;"+
			"uniform mat4 u_model_matrix;"+
			"uniform mat4 u_view_matrix;"+
			"uniform mat4 u_perspective_projection_matrix;"+
			"uniform vec3 u_la_red;"+
			"uniform vec3 u_ld_red;"+
			"uniform vec3 u_ls_red;"+
			"uniform vec4 u_light_position_red;"+
			"uniform vec3 u_la_blue;"+
			"uniform vec3 u_ld_blue;"+
			"uniform vec3 u_ls_blue;"+
			"uniform vec4 u_light_position_blue;"+
			"uniform vec3 u_la_green;"+
			"uniform vec3 u_ld_green;"+
			"uniform vec3 u_ls_green;"+
			"uniform vec4 u_light_position_green;"+
			"uniform vec3 u_ka;"+
			"uniform vec3 u_kd;"+
			"uniform vec3 u_ks;"+
			"uniform int u_LKeyPressed;"+
			"uniform float u_shininess;"+
			"out vec3 phong_ads_light;"+
			"void main(void)"+
			"{"+
				"if(u_LKeyPressed == 1)"+
				"{"+
					"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
					"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
					"vec3 view_vector = normalize(-eye_coordinates.xyz);"+

					"vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));"+
					"vec3 reflection_vector_red = reflect(-light_direction_red, transformed_normal);"+
					"vec3 ambient_red = u_la_red * u_ka;"+
					"vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red, transformed_normal), 0.0f);"+
					"vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, view_vector), 0.0f), u_shininess);"+

					"vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));"+
					"vec3 reflection_vector_blue = reflect(-light_direction_blue, transformed_normal);"+
					"vec3 ambient_blue = u_la_blue * u_ka;"+
					"vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue, transformed_normal), 0.0f);"+
					"vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, view_vector), 0.0f), u_shininess);"+

					"vec3 light_direction_green = normalize(vec3(u_light_position_green - eye_coordinates));"+
					"vec3 reflection_vector_green = reflect(-light_direction_green, transformed_normal);"+
					"vec3 ambient_green = u_la_green * u_ka;"+
					"vec3 diffuse_green = u_ld_green * u_kd * max(dot(light_direction_green, transformed_normal), 0.0f);"+
					"vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green, view_vector), 0.0f), u_shininess);"+

					"phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + diffuse_green + specular_red + specular_blue + specular_green;"+		
				"}"+
				"else"+
				"{"+
					"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
				"}"+
				"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
			"}"
		);

		// provide source code to shader
		GLES32.glShaderSource(vertexShaderObject_PV, vertexShaderSourceCode_PV);

		// compile shader & check for errors
		GLES32.glCompileShader(vertexShaderObject_PV);
		int[] iShaderCompiledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject_PV, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
		
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject_PV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
            if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_PV);
				System.out.println("SSK: Vertex Shader Compilation Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		//**********************************************
		// Fragment Shader
		//**********************************************
		// Create Shader
		//*********************************************
		fragmentShaderObject_PV = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		// fragment shader source code
		final String fragmentShaderSourceCode_PV = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec3 phong_ads_light;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
			"FragColor = vec4(phong_ads_light, 1.0f);"+
			"}"
		);

		// provide source code to shader
		GLES32.glShaderSource(fragmentShaderObject_PV, fragmentShaderSourceCode_PV);

		// compile shader and check for errors
		GLES32.glCompileShader(fragmentShaderObject_PV);

		iShaderCompiledStatus[0] = 0; // re-initialize
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize

		GLES32.glGetShaderiv(fragmentShaderObject_PV, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
		
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject_PV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
            if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_PV);
				System.out.println("SSK: Fragment Shader Compilation Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		// create shader program
		shaderProgramObject_PV = GLES32.glCreateProgram();

		// attach vertex shader to shader program
		GLES32.glAttachShader(shaderProgramObject_PV, vertexShaderObject_PV);

		// attach fragment shader to shader program
		GLES32.glAttachShader(shaderProgramObject_PV, fragmentShaderObject_PV);

		// pre-link binding of shader program object with vertex shader attributes
		GLES32.glBindAttribLocation(shaderProgramObject_PV, GLESMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject_PV, GLESMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");
		
        // link the two shaders together to shader program object
		GLES32.glLinkProgram(shaderProgramObject_PV);

		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize

		GLES32.glGetProgramiv(shaderProgramObject_PV, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
		
		if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(shaderProgramObject_PV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_PV);
				System.out.println("SSK: Shader Program Link Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		// get MVP uniform location
		modelMatrixUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_model_matrix");
    	viewMatrixUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_view_matrix");
    	perspectiveProjectionUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_perspective_projection_matrix");
    	LaUniform_Red_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_la_red");
    	LdUniform_Red_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ld_red");
   		LsUniform_Red_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ls_red");
    	lightPositionUniform_Red_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_light_position_red");
    	LaUniform_Blue_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_la_blue");
    	LdUniform_Blue_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ld_blue");
   		LsUniform_Blue_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ls_blue");
    	lightPositionUniform_Blue_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_light_position_blue");
    	LaUniform_Green_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_la_green");
    	LdUniform_Green_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ld_green");
   		LsUniform_Green_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ls_green");
    	lightPositionUniform_Green_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_light_position_green");
    	KaUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ka");
    	KdUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_kd");
    	KsUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_ks");
    	KshineUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_shininess");
    	LKeyPressedUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV,"u_LKeyPressed");

//---------------------------------------------------------------------------------------------------------------------------------------------

        vertexShaderObject_PF = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		// vertex shader source code
		final String vertexShaderSourceCode_PF = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec3 vNormal;"+
			"uniform mat4 u_model_matrix;"+
			"uniform mat4 u_view_matrix;"+
			"uniform mat4 u_perspective_projection_matrix;"+
			"uniform vec4 u_light_position_red;"+
			"uniform vec4 u_light_position_blue;"+
			"uniform vec4 u_light_position_green;"+
			"uniform int u_LKeyPressed;"+
			"out vec3 transformed_normal;"+
			"out vec3 view_vector;"+
			"out vec3 light_direction_red;"+
			"out vec3 light_direction_blue;"+
			"out vec3 light_direction_green;"+
			"void main(void)"+
			"{"+
				"if(u_LKeyPressed == 1)"+
				"{"+
					"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
					"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
					"view_vector = -eye_coordinates.xyz;"+
					"light_direction_red = vec3(u_light_position_red - eye_coordinates);"+
					"light_direction_blue = vec3(u_light_position_blue - eye_coordinates);"+
					"light_direction_green = vec3(u_light_position_green - eye_coordinates);"+
				"}"+
				"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
			"}"
		);

		// provide source code to shader
		GLES32.glShaderSource(vertexShaderObject_PF, vertexShaderSourceCode_PF);

		// compile shader & check for errors
		GLES32.glCompileShader(vertexShaderObject_PF);
		iShaderCompiledStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject_PF, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
		
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject_PF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
            if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_PF);
				System.out.println("SSK: Vertex Shader Compilation Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		//**********************************************
		// Fragment Shader
		//**********************************************
		// Create Shader
		//*********************************************
		fragmentShaderObject_PF = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		
		final String fragmentShaderSourceCode_PF = String.format(
        "#version 320 es"+
        "\n"+
        "precision highp float;"+
        "in vec3 transformed_normal;"+
        "in vec3 view_vector;"+
        "in vec3 light_direction_red;"+
        "in vec3 light_direction_blue;"+
        "in vec3 light_direction_green;"+
        "uniform vec3 u_la_red;"+
        "uniform vec3 u_ld_red;"+
        "uniform vec3 u_ls_red;"+
        "uniform vec3 u_la_blue;"+
        "uniform vec3 u_ld_blue;"+
        "uniform vec3 u_ls_blue;"+
        "uniform vec3 u_la_green;"+
        "uniform vec3 u_ld_green;"+
        "uniform vec3 u_ls_green;"+
        "uniform vec3 u_ka;"+
        "uniform vec3 u_kd;"+
        "uniform vec3 u_ks;"+
        "uniform int u_LKeyPressed;"+
        "uniform float u_shininess;"+
        "out vec4 FragColor;"+
        "void main(void)"+
        "{"+
	        "vec3 phong_ads_light;"+
	        "if(u_LKeyPressed == 1)"+
	        "{"+
		        "vec3 normalized_transformed_normal      = normalize(transformed_normal);"+
		        "vec3 normalized_light_direction_red     = normalize(light_direction_red);"+
		        "vec3 normalized_light_direction_blue    = normalize(light_direction_blue);"+
		        "vec3 normalized_light_direction_green   = normalize(light_direction_green);"+
		        "vec3 normalized_view_vector             = normalize(view_vector);"+

		        "vec3 reflection_vector_red = reflect(-normalized_light_direction_red, normalized_transformed_normal);"+
		        "vec3 ambient_red = u_la_red * u_ka;"+
		        "vec3 diffuse_red = u_ld_red * u_kd * max(dot(normalized_light_direction_red, normalized_transformed_normal), 0.0f);"+
		        "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, normalized_view_vector), 0.0f), u_shininess);"+

		        "vec3 reflection_vector_blue = reflect(-normalized_light_direction_blue, normalized_transformed_normal);"+
		        "vec3 ambient_blue = u_la_blue * u_ka;"+
		        "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(normalized_light_direction_blue, normalized_transformed_normal), 0.0f);"+
		        "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, normalized_view_vector), 0.0f), u_shininess);"+

		        "vec3 reflection_vector_green = reflect(-normalized_light_direction_green, normalized_transformed_normal);"+
		        "vec3 ambient_green = u_la_green * u_ka;"+
		        "vec3 diffuse_green = u_ld_green * u_kd * max(dot(normalized_light_direction_green, normalized_transformed_normal), 0.0f);"+
		        "vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green, normalized_view_vector), 0.0f), u_shininess);"+

		        "phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + diffuse_green + specular_red + specular_blue + specular_green;"+
	        "}"+
	        "else"+
	        "{"+
	        	"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
	        "}"+
	        "FragColor = vec4(phong_ads_light, 1.0f);"+
        "}"
		);

		// provide source code to shader
		GLES32.glShaderSource(fragmentShaderObject_PF, fragmentShaderSourceCode_PF);

		// compile shader and check for errors
		GLES32.glCompileShader(fragmentShaderObject_PF);

		iShaderCompiledStatus[0] = 0; // re-initialize
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize

		GLES32.glGetShaderiv(fragmentShaderObject_PF, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
		
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject_PF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
            if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_PF);
				System.out.println("SSK: Fragment Shader Compilation Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		// create shader program
		shaderProgramObject_PF = GLES32.glCreateProgram();

		// attach vertex shader to shader program
		GLES32.glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);

		// attach fragment shader to shader program
		GLES32.glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

		GLES32.glBindAttribLocation(shaderProgramObject_PF, GLESMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject_PF, GLESMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

		GLES32.glLinkProgram(shaderProgramObject_PF);

		iShaderProgramLinkStatus[0] = 0;
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize

		GLES32.glGetProgramiv(shaderProgramObject_PF, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
		
		if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(shaderProgramObject_PF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_PF);
				System.out.println("SSK: Shader Program Link Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		// get MVP uniform location
		modelMatrixUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_model_matrix");
    	viewMatrixUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_view_matrix");
    	perspectiveProjectionUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_perspective_projection_matrix");
    	LaUniform_Red_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_la_red");
    	LdUniform_Red_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ld_red");
   		LsUniform_Red_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ls_red");
    	lightPositionUniform_Red_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_light_position_red");
    	LaUniform_Blue_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_la_blue");
    	LdUniform_Blue_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ld_blue");
   		LsUniform_Blue_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ls_blue");
    	lightPositionUniform_Blue_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_light_position_blue");
    	LaUniform_Green_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_la_green");
    	LdUniform_Green_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ld_green");
   		LsUniform_Green_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ls_green");
    	lightPositionUniform_Green_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_light_position_green");
    	KaUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ka");
    	KdUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_kd");
    	KsUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_ks");
    	KshineUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_shininess");
    	LKeyPressedUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF,"u_LKeyPressed");

//---------------------------------------------------------------------------------------------------------------------------------------------

		Sphere sphere = new Sphere();
		float sphereVertices[] = new float[1146];
		float sphereNormals[]  = new float[1146];
		float sphereTextures[] = new float[764];
		short sphereElements[] = new short[2280];

		sphere.getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);

		numVertices = sphere.getNumberOfSphereVertices();
		numElements = sphere.getNumberOfSphereElements();

		// vao
		GLES32.glGenVertexArrays(1, vao_sphere, 0);
		GLES32.glBindVertexArray(vao_sphere[0]);

			// position vbo
			GLES32.glGenBuffers(1, vbo_sphere_position, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);

				ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphereVertices.length * 4);	// 4 = sizeof float
				byteBuffer.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
				verticesBuffer.put(sphereVertices);
				verticesBuffer.position(0);

				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// normal vbo
			GLES32.glGenBuffers(1, vbo_sphere_normal, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);

					byteBuffer = ByteBuffer.allocateDirect(sphereNormals.length * 4);	// 4 = sizeof float
					byteBuffer.order(ByteOrder.nativeOrder());
					FloatBuffer normalBuffers = byteBuffer.asFloatBuffer();
					normalBuffers.put(sphereNormals);
					normalBuffers.position(0);

					GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereNormals.length * 4, normalBuffers, GLES32.GL_STATIC_DRAW);
					GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
					GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_NORMAL);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


			// element vbo
			GLES32.glGenBuffers(1, vbo_sphere_element, 0);
			GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

				byteBuffer = ByteBuffer.allocateDirect(sphereElements.length * 2);	// 4 = sizeof float
				byteBuffer.order(ByteOrder.nativeOrder());
				ShortBuffer elementsBuffers = byteBuffer.asShortBuffer();
				elementsBuffers.put(sphereElements);
				elementsBuffers.position(0);

				GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, sphereElements.length * 2, elementsBuffers, GLES32.GL_STATIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0); 

		// enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);

		// depth test to do
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		// We will always cull back faces for better performance
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		// Set the background color
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// set projectionMatrix to identity matrix
		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	private void resize(int width, int height)
	{
		// code

		if(height <= 0)
		{
			height = 1;
		}

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void draw()
	{
		float lightPosition[] 		= 	{100.0f, 100.0f, 100.0f, 1.0f};
		float lightAmbient[] 		= 	{0.1f, 0.1f, 0.1f, 1.0f};
		float lightDiffuse[] 		= 	{0.5f, 0.2f, 0.7f, 1.0f};
		float lightSpecular[]		= 	{0.7f, 0.7f, 0.7f, 1.0f};

		float MaterialAmbient[] 	= 	{0.0f, 0.0f, 0.0f, 0.0f};
		float MaterialDiffuse[] 	= 	{1.0f, 1.0f, 1.0f, 1.0f};
		float MaterialSpecular[] 	= 	{1.0f, 1.0f, 1.0f, 1.0f};

		float MaterialShininess 	= 	128.0f;

		// Draw background color
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		if(tapOnScreen == 1)
		{
			/////////////////////////////////////////////////////////////////////////////
			// PER VERTEX DRAWING CODE
			////////////////////////////////////////////////////////////////////////////
			// use shader program
			GLES32.glUseProgram(shaderProgramObject_PV);
			if(bLight == true)
			{
				lightPosition_Red_PF[0] = 0.0f;
				lightPosition_Red_PF[1] = factor * (float)Math.sin(angleX);
				lightPosition_Red_PF[2] = factor * (float)Math.cos(angleX);
				lightPosition_Red_PF[3] = 1.0f;

				lightPosition_Blue_PF[0] = factor * (float)Math.sin(angleZ);
				lightPosition_Blue_PF[1] = factor * (float)Math.sin(angleZ);
				lightPosition_Blue_PF[2] = 0.0f;
				lightPosition_Blue_PF[3] = 1.0f;

				lightPosition_Green_PF[0] = factor * (float)Math.sin(angleY);
				lightPosition_Green_PF[1] = 0.0f;
				lightPosition_Green_PF[2] = factor * (float)Math.cos(angleY);
				lightPosition_Green_PF[3] = 1.0f;

				GLES32.glUniform1i(LKeyPressedUniform_PF, 1);
				GLES32.glUniform1f(KshineUniform_PF, MaterialShininess_PF);

				GLES32.glUniform4fv(lightPositionUniform_Red_PF, 1, lightPosition_Red_PF, 0);
				GLES32.glUniform3fv(LaUniform_Red_PF, 1, lightAmbient_Red_PF, 0);
				GLES32.glUniform3fv(LdUniform_Red_PF, 1, lightDiffuse_Red_PF, 0);
				GLES32.glUniform3fv(LsUniform_Red_PF, 1, lightSpecular_Red_PF, 0);

				GLES32.glUniform4fv(lightPositionUniform_Blue_PF, 1, lightPosition_Blue_PF, 0);
				GLES32.glUniform3fv(LaUniform_Blue_PF, 1, lightAmbient_Blue_PF, 0);
				GLES32.glUniform3fv(LdUniform_Blue_PF, 1, lightDiffuse_Blue_PF, 0);
				GLES32.glUniform3fv(LsUniform_Blue_PF, 1, lightSpecular_Blue_PF, 0);

				GLES32.glUniform4fv(lightPositionUniform_Green_PF, 1, lightPosition_Green_PF, 0);
				GLES32.glUniform3fv(LaUniform_Green_PF, 1, lightAmbient_Green_PF, 0);
				GLES32.glUniform3fv(LdUniform_Green_PF, 1, lightDiffuse_Green_PF, 0);
				GLES32.glUniform3fv(LsUniform_Green_PF, 1, lightSpecular_Green_PF, 0);

				GLES32.glUniform3fv(KaUniform_PF, 1, MaterialAmbient_PF, 0);
				GLES32.glUniform3fv(KdUniform_PF, 1, MaterialDiffuse_PF, 0);
				GLES32.glUniform3fv(KsUniform_PF, 1, MaterialSpecular_PF, 0);
	 		}
	 		else
	 		{
	 			GLES32.glUniform1i(LKeyPressedUniform_PF, 0);
	 		}

			// OpenGL-ES drawing
			float modelMatrix[] = new float[16];
			float viewMatrix[] = new float[16];
			float projectionMatrix[] = new float[16];
			float translateMatrix[] = new float[16];

			// set modelview & modelviewprojection matrices to identity
			Matrix.setIdentityM(modelMatrix, 0);
			Matrix.setIdentityM(viewMatrix, 0);
			Matrix.setIdentityM(projectionMatrix, 0);
			Matrix.setIdentityM(translateMatrix, 0);

			Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);

			modelMatrix = translateMatrix;
			projectionMatrix = perspectiveProjectionMatrix;

			
			// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
			// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
			
            GLES32.glUniformMatrix4fv(modelMatrixUniform_PF, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(viewMatrixUniform_PF, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(perspectiveProjectionUniform_PF, 1, false, projectionMatrix, 0);
		}
		else
		{
			/////////////////////////////////////////////////////////////////////////////
			// PER FRAGMENT DRAWING CODE
			////////////////////////////////////////////////////////////////////////////
			// use shader program
			GLES32.glUseProgram(shaderProgramObject_PV);

			if(bLight == true)
			{
				lightPosition_Red_PV[0] = 0.0f;
				lightPosition_Red_PV[1] = factor * (float)Math.sin(angleX);
				lightPosition_Red_PV[2] = factor * (float)Math.cos(angleX);
				lightPosition_Red_PV[3] = 1.0f;

                lightPosition_Green_PV[0] = factor * (float)Math.sin(angleY);
				lightPosition_Green_PV[1] = 0.0f;
				lightPosition_Green_PV[2] = factor * (float)Math.cos(angleY);
				lightPosition_Green_PV[3] = 1.0f;

				lightPosition_Blue_PV[0] = factor * (float)Math.sin(angleZ);
				lightPosition_Blue_PV[1] = factor * (float)Math.sin(angleZ);
				lightPosition_Blue_PV[2] = 0.0f;
				lightPosition_Blue_PV[3] = 1.0f;

				

				GLES32.glUniform1i(LKeyPressedUniform_PV, 1);
				GLES32.glUniform1f(KshineUniform_PV, MaterialShininess_PV);

				GLES32.glUniform4fv(lightPositionUniform_Red_PV, 1, lightPosition_Red_PV, 0);
				GLES32.glUniform3fv(LaUniform_Red_PV, 1, lightAmbient_Red_PV, 0);
				GLES32.glUniform3fv(LdUniform_Red_PV, 1, lightDiffuse_Red_PV, 0);
				GLES32.glUniform3fv(LsUniform_Red_PV, 1, lightSpecular_Red_PV, 0);

				GLES32.glUniform4fv(lightPositionUniform_Blue_PV, 1, lightPosition_Blue_PV, 0);
				GLES32.glUniform3fv(LaUniform_Blue_PV, 1, lightAmbient_Blue_PV, 0);
				GLES32.glUniform3fv(LdUniform_Blue_PV, 1, lightDiffuse_Blue_PV, 0);
				GLES32.glUniform3fv(LsUniform_Blue_PV, 1, lightSpecular_Blue_PV, 0);

				GLES32.glUniform4fv(lightPositionUniform_Green_PV, 1, lightPosition_Green_PV, 0);
				GLES32.glUniform3fv(LaUniform_Green_PV, 1, lightAmbient_Green_PV, 0);
				GLES32.glUniform3fv(LdUniform_Green_PV, 1, lightDiffuse_Green_PV, 0);
				GLES32.glUniform3fv(LsUniform_Green_PV, 1, lightSpecular_Green_PV, 0);

				GLES32.glUniform3fv(KaUniform_PV, 1, MaterialAmbient_PV, 0);
				GLES32.glUniform3fv(KdUniform_PV, 1, MaterialDiffuse_PV, 0);
				GLES32.glUniform3fv(KsUniform_PV, 1, MaterialSpecular_PV, 0);
	 		}
	 		else
	 		{
	 			GLES32.glUniform1i(LKeyPressedUniform_PV, 0);
	 		}

			// OpenGL-ES drawing
			float modelMatrix[] = new float[16];
			float viewMatrix[] = new float[16];
			float projectionMatrix[] = new float[16];
			float translateMatrix[] = new float[16];

			// set modelview & modelviewprojection matrices to identity
			Matrix.setIdentityM(modelMatrix, 0);
			Matrix.setIdentityM(viewMatrix, 0);
			Matrix.setIdentityM(projectionMatrix, 0);
			Matrix.setIdentityM(translateMatrix, 0);

			Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);

			modelMatrix = translateMatrix;
			projectionMatrix = perspectiveProjectionMatrix;

			
			// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
			// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
			
            GLES32.glUniformMatrix4fv(modelMatrixUniform_PV, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(viewMatrixUniform_PV, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(perspectiveProjectionUniform_PV, 1, false, projectionMatrix, 0);
		}

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		// bind vao
		GLES32.glBindVertexArray(0);
		GLES32.glUseProgram(0);

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

		// render/flush
		requestRender();
	}

	void unInitialize()
	{

        // destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }

		if(shaderProgramObject_PV != 0)
		{
			if(vertexShaderObject_PV != 0)
			{
				GLES32.glDetachShader(shaderProgramObject_PV,vertexShaderObject_PV);
				GLES32.glDeleteShader(vertexShaderObject_PV);
				vertexShaderObject_PV = 0;
			}

			if(fragmentShaderObject_PV != 0)
			{
				GLES32.glDetachShader(shaderProgramObject_PV,fragmentShaderObject_PV);
				GLES32.glDeleteShader(fragmentShaderObject_PV);
				fragmentShaderObject_PV = 0;
			}
		}

		if(shaderProgramObject_PF != 0)
		{
			if(vertexShaderObject_PF != 0)
			{
				GLES32.glDetachShader(shaderProgramObject_PF,vertexShaderObject_PF);
				GLES32.glDeleteShader(vertexShaderObject_PF);
				vertexShaderObject_PF = 0;
			}

			if(fragmentShaderObject_PF != 0)
			{
				GLES32.glDetachShader(shaderProgramObject_PF,fragmentShaderObject_PF);
				GLES32.glDeleteShader(fragmentShaderObject_PF);
				fragmentShaderObject_PF = 0;
			}
		}

		if(shaderProgramObject_PV != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObject_PV);
			shaderProgramObject_PV = 0;
		}

		if(shaderProgramObject_PF != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObject_PF);
			shaderProgramObject_PF = 0;
		}		
	}
}
