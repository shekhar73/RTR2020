package com.example.roboticarm;

import android.content.Context;		// for drawing context related
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.opengl.GLSurfaceView;	// for OpenGL Surface View and all related
import android.opengl.GLES32;	// for OpenGLES 3.2
import javax.microedition.khronos.opengles.GL10;	// for OpenGLES 1.0 needed as param type EGLConfig
import javax.microedition.khronos.egl.EGLConfig;	// for EGLConfig needed as param type EGLConfig

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix;	// for Matrix math

// A view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;

	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int gShaderProgramObject;

	boolean bLighting = true;

	private int laUniform;
	private int ldUniform;
	private int lsUniform;
	private int kaUniform;
	private int kdUniform;
	private int ksUniform;
	private int materialShininessUniform;

	private int lightPositionUniform;
	private int lKeyIsPressedUniform;
	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int projectionMatrixUniform;

	private float[] lightAmbient = new float[] { 0.0f, 0.0f, 0.0f, 0.0f };
	private float[] lightDiffuse = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
	private float[] lightSpecular = new float[] { 0.7f, 0.7f, 0.7f, 1.0f };
	private float[] lightPosition = new float[] { 0.0f, 0.0f, 5.0f, 1.0f };

	private float materialAmbient[] = new float[] { 0.0f, 0.0f, 0.0f, 0.0f };
	private float materialDiffuse[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
	private float materialSpecular[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
	private float materialShininess = 50.0f;
	
	private int shoulder = 0;
	private int elbow = 0;
	
	private boolean on = true;

	private int[] vao = new int[1];
	private int[] vbo = new int[1];
	private int mvpUniform;

	private float perspectiveProjectionMatrix[] = new float[16];	// 4x4 matrix

	private int[] vao_sphere = new int[1];
	private int[] vbo_position_sphere = new int[1];
	private int[] vbo_normal_sphere = new int[1];
	private int[] vbo_element_sphere = new int[1];

	private int numElements = 0;
	private int numVertices = 0;
	private float angleSphere = 0.0f;


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
		System.out.println("SSK: "+"Double Tap");
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		if (on) 
		{
			shoulder = (shoulder + 6) % 360;
		}
		else 
		{
			elbow = (elbow + 6) % 360;
		}
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		if (on) 
		{
			shoulder = (shoulder + 6) % 360;
		}
		else 
		{
			elbow = (elbow + 6) % 360;
		}
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
		on = !on;
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
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		final String vertexShaderSourceCode = String.format(
		
		"#version 320 es" +
		"\n" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_l_key_is_pressed;" +
		"uniform vec4 u_light_position;" +
		"out vec3 t_norm;" +
		"out vec3 light_direction;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if (u_l_key_is_pressed == 1)" +
		"{" +
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" +
		"t_norm = mat3(u_view_matrix * u_model_matrix) * vNormal;" +
		"light_direction = vec3(u_light_position - eye_coordinates);" +
		"viewer_vector = vec3(-eye_coordinates.xyz);" +
		"}" +
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
		"}"
		
		);

		// provide source code to shader
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

		// compile shader & check for errors
		GLES32.glCompileShader(vertexShaderObject);

		int[] iShaderComipledStatus = new int[1];
		int[] iProgramLinkStatus = new int[1];
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
		("#version 320 es" +
		"\n" +
		"precision highp float;" +
		"precision highp int;" +
		"in vec3 t_norm;" +
		"in vec3 viewer_vector;" +
		"in vec3 light_direction;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_material_shininess;" +
		"uniform int u_l_key_is_pressed;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"if (u_l_key_is_pressed == 1)" +
		"{" +
		"vec3 normalized_viewer_vector = normalize(viewer_vector);" +
		"vec3 normalized_light_direction = normalize(light_direction);" +
		"vec3 normalized_t_norm = normalize(t_norm);" +
		"float tn_dot_light_direction = max(dot(normalized_light_direction, normalized_t_norm), 0.0);" +
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_t_norm);" +
		"vec3 ambient = u_la * u_ka;" +
		"vec3 diffuse = u_ld * u_kd * tn_dot_light_direction;" +
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" +
		"vec3 phong_ads_light = ambient + diffuse + specular;" +
		"FragColor = vec4(phong_ads_light, 1.0);" +
		"}" +
		"else" +
		"{" +
		"FragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
		"}" +
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
		GLES32.glBindAttribLocation(gShaderProgramObject, GLESMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(gShaderProgramObject, GLESMacros.SSK_ATTRIBUTE_COLOR, "vColor");
		GLES32.glBindAttribLocation(gShaderProgramObject, GLESMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");
		

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
		modelMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
		viewMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
		projectionMatrixUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
		lKeyIsPressedUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_l_key_is_pressed");
		laUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_la");
		ldUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_ld");
		lsUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_ls");
		kaUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_ka");
		kdUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_kd");
		ksUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_ks");
		materialShininessUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
		lightPositionUniform = GLES32.glGetUniformLocation(gShaderProgramObject, "u_light_position");
		
		// *** vertices, colors, shader attribs, vbo, vao initializations ***
		GLES32.glGenVertexArrays(1, vao_sphere, 0);
		GLES32.glBindVertexArray(vao_sphere[0]);
		
		Sphere sphere = new Sphere();
		float[] sphere_vertices = new float[1146];
		float[] sphere_normals = new float[1146];
		float[] sphere_textures = new float[764];
		short[] sphere_elements = new short[2280];



		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		numVertices = sphere.getNumberOfSphereVertices();
		numElements = sphere.getNumberOfSphereElements();
		
		GLES32.glGenBuffers(1, vbo_position_sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_sphere[0]);
		
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(sphere_vertices);
		positionBuffer.position(0); 				
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_vertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
				
		GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_POSITION);			
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// Normals
		GLES32.glGenBuffers(1, vbo_normal_sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_sphere[0]);
		
		byteBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(sphere_normals);
		positionBuffer.position(0); 
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_normals.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
				
		GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_NORMAL);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// Elements
		GLES32.glGenBuffers(1, vbo_element_sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);
		
		byteBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
		byteBuffer.order(ByteOrder.nativeOrder());
		ShortBuffer elementBuffer = byteBuffer.asShortBuffer();
		elementBuffer.put(sphere_elements);
		elementBuffer.position(0); 
		
		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, sphere_elements.length * 2, elementBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
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

		float[] viewMatrix = new float[16];
		float[] modelMatrix = new float[16];
		float[] modelMatrixT = new float[16];
		float[] translationMatrix = new float[16];
		float[] scaleMatrix = new float[16];
		float[] rotationMatrix = new float[16];
		float[] rotationMatrixX = new float[16];
		float[] rotationMatrixY = new float[16];
		float[] rotationMatrixZ = new float[16];
		float[] shoulderPosition = new float[16];
		float[] modelViewProjectionMatrix = new float[16];
		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(modelMatrixT, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(shoulderPosition, 0);
		Matrix.setIdentityM(rotationMatrix, 0);
		Matrix.setIdentityM(rotationMatrixX, 0);
		Matrix.setIdentityM(rotationMatrixY, 0);
		Matrix.setIdentityM(rotationMatrixZ, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		Matrix.translateM(modelMatrix, 0, -2.0f, 0.0f, -8.0f);
		
		Matrix.setRotateM(rotationMatrix, 0, (float)shoulder, 0.0f, 0.0f, 1.0f);
		Matrix.translateM(modelMatrixT, 0, 1.0f, 0.0f, 0.0f);
		
		Matrix.scaleM(scaleMatrix, 0, 2.0f, 0.5f, 1.0f);
		
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrix, 0, modelMatrixT, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotationMatrix, 0);
		
		Matrix.multiplyMM(shoulderPosition, 0, shoulderPosition, 0, modelMatrix, 0);
		
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);
					
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);	
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);	
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);	
		
		if (bLighting == true) {
			GLES32.glUniform1i(lKeyIsPressedUniform, 1);

			GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, materialShininess);
		}
		else {
			GLES32.glUniform1i(lKeyIsPressedUniform, 0);
		}
		
		GLES32.glBindVertexArray(vao_sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(modelMatrixT, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);
		Matrix.setIdentityM(rotationMatrixX, 0);
		Matrix.setIdentityM(rotationMatrixY, 0);
		Matrix.setIdentityM(rotationMatrixZ, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		Matrix.translateM(modelMatrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(modelMatrix, 0, shoulderPosition, 0, modelMatrix, 0);
		
		Matrix.setRotateM(rotationMatrix, 0, (float)elbow, 0.0f, 0.0f, 1.0f);
		Matrix.translateM(modelMatrixT, 0, 1.0f, 0.0f, 0.0f);
		
		Matrix.scaleM(scaleMatrix, 0, 2.0f, 0.5f, 1.0f);
		
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrix, 0, modelMatrixT, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotationMatrix, 0);
				
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);
					
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);	
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);	
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);	

		GLES32.glBindVertexArray(vao_sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);		

		GLES32.glUseProgram(0);

		// render/flush
		requestRender();
	}

	void unInitialize()
	{
		// code
		// destroy vao
		if(vao[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
		}

		// destroy vao
		if(vbo[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo, 0);
			vbo[0] = 0;
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

