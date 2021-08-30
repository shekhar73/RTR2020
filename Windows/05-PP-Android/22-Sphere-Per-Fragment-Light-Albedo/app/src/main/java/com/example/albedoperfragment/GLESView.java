package com.example.albedoperfragment;

import android.content.Context;		// for drawing context related
import android.opengl.GLSurfaceView;	// for OpenGL Surface View and all related
import javax.microedition.khronos.opengles.GL10;	// for OpenGLES 1.0 needed as param type EGLConfig
import javax.microedition.khronos.egl.EGLConfig;	// for EGLConfig needed as param type EGLConfig

import android.opengl.GLES32;	// for OpenGLES 3.2
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

import android.opengl.Matrix;	// for Matrix math

// A view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;

	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;

	private int[] vao_sphere = new int[1];
	private int[] vbo_sphere_position = new int[1];
	private int[] vbo_sphere_normal = new int[1];
	private int[] vbo_sphere_element = new int[1];
	
	int numVertices;
	int numElements;

	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int perspectiveProjectionUniform;

	private int LaUniform;
	private int LdUniform;
	private int LsUniform;
	private int lightPositionUniform;

	private int KaUniform;
	private int KdUniform;
	private int KsUniform;
	private int KshineUniform;
	
	private int LKeyPressedUniform;

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
		tapOnScreen = 1;
		System.out.println("SSK: "+"Double Tap");
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
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
		unInitialize();
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
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		// vertex shader source code
		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec3 vNormal;"+
			"uniform mat4 u_model_matrix;"+
			"uniform mat4 u_view_matrix;"+
			"uniform mat4 u_perspective_projection_matrix;"+
			"uniform vec4 u_light_position;"+
			"out vec3 transformed_normal;"+
			"out vec3 light_direction;"+
			"out vec3 view_vector;"+
			"void main(void)"+
			"{"+
				"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
				"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
				"light_direction = vec3(u_light_position - eye_coordinates);"+
				"view_vector = -eye_coordinates.xyz;"+
				"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
			"}"
		);

		// provide source code to shader
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

		// compile shader & check for errors
		GLES32.glCompileShader(vertexShaderObject);
		int[] iShaderComipledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderComipledStatus, 0);

        if(iShaderComipledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			
            if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
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
		fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		// fragment shader source code
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec3 transformed_normal;"+
			"in vec3 light_direction;"+
			"in vec3 view_vector;"+
			"uniform vec3 u_la;"+
			"uniform vec3 u_ld;"+
			"uniform vec3 u_ls;"+
			"uniform vec3 u_ka;"+
			"uniform vec3 u_kd;"+
			"uniform vec3 u_ks;"+
            "uniform highp float u_shininess;"+
			"uniform int u_LKeyPressed;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
				"vec3 phong_ads_light;"+
				"if(u_LKeyPressed == 1)"+
				"{"+
					"vec3 normalized_transformed_normal = normalize(transformed_normal);"+
					"vec3 normalized_light_direction = normalize(light_direction);"+
					"vec3 normalized_view_vector = normalize(view_vector);"+
					"vec3 ambient = u_la * u_ka;"+
					"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"+
					"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
					"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_shininess);"+
					"phong_ads_light = ambient + diffuse + specular;"+
				"}"+
				"else"+
				"{"+
					"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
				"}"+
				"FragColor = vec4(phong_ads_light, 1.0f);"+
			"}"
		);

		// provide source code to shader
		GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

		// compile shader and check for errors
		GLES32.glCompileShader(fragmentShaderObject);

		iShaderComipledStatus[0] = 0; // re-initialize
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize

		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderComipledStatus, 0);

		if(iShaderComipledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("SSK: Fragment Shader Compilation Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		// create shader program
		shaderProgramObject = GLES32.glCreateProgram();

		// attach vertex shader to shader program
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

		// attach fragment shader to shader program
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

		// pre-link binding of shader program object with vertex shader attributes
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");
		// link the two shaders together to shader program object
		GLES32.glLinkProgram(shaderProgramObject);

		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize
		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
		if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject);
				System.out.println("SSK: Shader Program Link Log = "+szInfoLog);
				unInitialize();
				System.exit(0);
			}
		}

		// get MVP uniform location
		modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
		viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
		perspectiveProjectionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_perspective_projection_matrix");
		LaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la");
		LdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		LsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");
		KaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
		KdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		KsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");
		KshineUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_shininess");
		LKeyPressedUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_LKeyPressed");


		Sphere sphere = new Sphere();
		float sphereVertices[] = new float[1146];
		float sphereNormlas[]  = new float[1146];
		float sphereTextures[] = new float[764];
		short sphereElements[] = new short[2280];

		sphere.getSphereVertexData(sphereVertices, sphereNormlas, sphereTextures, sphereElements);

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

					byteBuffer = ByteBuffer.allocateDirect(sphereNormlas.length * 4);	// 4 = sizeof float
					byteBuffer.order(ByteOrder.nativeOrder());
					FloatBuffer normalBuffers = byteBuffer.asFloatBuffer();
					normalBuffers.put(sphereNormlas);
					normalBuffers.position(0);

					GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereNormlas.length * 4, normalBuffers, GLES32.GL_STATIC_DRAW);
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
		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void draw()
	{
		// Draw background color
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// use shader program
		GLES32.glUseProgram(shaderProgramObject);

		if(bLight == true)
		{
			float lightPosition[] 		= 	{100.0f, 100.0f, 100.0f, 1.0f};
			float lightAmbient[] 		= 	{0.1f, 0.1f, 0.1f, 1.0f};
			float lightDiffuse[] 		= 	{0.5f, 0.2f, 0.7f, 1.0f};
			float lightSpecular[]		= 	{0.7f, 0.7f, 0.7f, 1.0f};

			float MaterialAmbient[] 	= 	{0.0f, 0.0f, 0.0f, 0.0f};
			float MaterialDiffuse[] 	= 	{1.0f, 1.0f, 1.0f, 1.0f};
			float MaterialSpecular[] 	= 	{1.0f, 1.0f, 1.0f, 1.0f};

			float MaterialShininess 	= 	128.0f;

			GLES32.glUniform1i(LKeyPressedUniform, 1);
			GLES32.glUniform1f(KshineUniform, MaterialShininess);
			GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

			GLES32.glUniform3fv(LaUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(LdUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(LsUniform, 1, lightSpecular, 0);

			GLES32.glUniform3fv(KaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(KdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(KsUniform, 1, MaterialSpecular, 0);
 		}
 		else
 		{
 			GLES32.glUniform1i(LKeyPressedUniform, 0);
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
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(perspectiveProjectionUniform, 1, false, projectionMatrix, 0);
		
		// bind vao
		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

		// unbind vao
		GLES32.glBindVertexArray(0);

		// un-use shader program
		GLES32.glUseProgram(0);

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

		if(shaderProgramObject != 0)
		{
			if(vertexShaderObject != 0)
			{
				// detach vertex shader from shader program object
				GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
				// delete vertex shader object
				GLES32.glDeleteShader(vertexShaderObject);
				vertexShaderObject = 0;
			}

			if(fragmentShaderObject != 0)
			{
				// detach fragment shader from shader program object
				GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
				// delete fragment shader object
				GLES32.glDeleteShader(fragmentShaderObject);
				fragmentShaderObject = 0;
			}

			// delete shader program object
			if(shaderProgramObject != 0)
			{
				GLES32.glDeleteProgram(shaderProgramObject);
				shaderProgramObject = 0;
			}
		}
	}
}

