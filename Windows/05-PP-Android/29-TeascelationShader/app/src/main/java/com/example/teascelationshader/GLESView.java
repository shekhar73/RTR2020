package com.example.teascelationshader;

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
	private int shaderProgramObject;

	// For Tesilation shaders
	private int teasilationControlShaderObject;
	private int teasilationEvalutionShaderObject;


	private int[] gVao = new int[1];
	private int[] gVbo_position = new int[1];

	// Uniforms for Teasilation Shaders
	private int numberOfSegmentsUniform;
	private int numberofStripsUniform;
	private int lineColorUniform;

	private int gMVPUniform;

	// Teasilation shaders
	private int uiNumberOfLineSegments;

	private float gPerspectiveProjectionMatrix[] = new float[16];	// 4x4 matrix

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
		uiNumberOfLineSegments--;
		if (uiNumberOfLineSegments <= 0)
		{
			uiNumberOfLineSegments = 1;

		}
		return (true);
	}

	// abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
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
		uiNumberOfLineSegments++;
		if (uiNumberOfLineSegments >= 50)
		{
			uiNumberOfLineSegments = 50;
		}
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

		// vertex shader source code
		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 vPosition;"+
			"void main(void)"+
			"{"+
				"gl_Position = vec4(vPosition);"+
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

	///////////////////////////////////////////////////////////////////////////////////////
	//*** TEASILATION SHADER ***
	//////////////////////////////////////////////////////////////////////////////////////
	teasilationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);

	final String teasilationShaderSourceCode = String.format
	(
		"#version 320 es"+
		"\n"+
		"layout(vertices = 4)out;"+
		"uniform int numberOfSegments;"+
		"uniform int numberOfStrips;"+
		"void main(void)"+
		"{"+
			"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"+
			"gl_TessLevelOuter[0] = float(numberOfStrips);"+
			"gl_TessLevelOuter[1] = float(numberOfSegments);"+
		"}"
	);

	// provide source code to shader
	GLES32.glShaderSource(teasilationControlShaderObject, teasilationShaderSourceCode);

	// compile shader and check for errors
	GLES32.glCompileShader(teasilationControlShaderObject);
	iShaderComipledStatus[0] = 0; // re-initialize
	iInfoLogLength[0] = 0; // re-initialize
	szInfoLog = null; // re-initialize
	GLES32.glGetShaderiv(teasilationControlShaderObject, GLES32.GL_COMPILE_STATUS, iShaderComipledStatus, 0);
	if(iShaderComipledStatus[0] == GLES32.GL_FALSE)
	{
		GLES32.glGetShaderiv(teasilationControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
		if(iInfoLogLength[0] > 0)
		{
			szInfoLog = GLES32.glGetShaderInfoLog(teasilationControlShaderObject);
			System.out.println("SSK: Fragment Shader Compilation Log = "+szInfoLog);
			unInitialize();
			System.exit(0);
		}
	}
	//---------------------------------------------------------------------------------------



	///////////////////////////////////////////////////////////////////////////////////////
	//*** TEASILATION EAVAULATION SHADER ***
	//////////////////////////////////////////////////////////////////////////////////////

	// Create Shader
	teasilationEvalutionShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);

	final String teasilationEvalutaionShaderSourceCode = String.format
	(
		"#version 320 es"+
		"\n"+
		"layout(isolines)in;"+
		"uniform mat4 u_mvp_matrix;"+
		"void main(void)"+
		"{"+
			"float tessCoord = gl_TessCoord.x;"+
			"vec3 P0 = gl_in[0].gl_Position.xyz;"+
			"vec3 P1 = gl_in[1].gl_Position.xyz;"+
			"vec3 P2 = gl_in[2].gl_Position.xyz;"+
			"vec3 P3 = gl_in[3].gl_Position.xyz;"+
			"vec3 P  = P0 * (1.0 - tessCoord) * (1.0 - tessCoord) * (1.0 - tessCoord) + P1 * 3.0 * tessCoord * (1.0 - tessCoord) * (1.0 - tessCoord) + P2 * 3.0 * tessCoord * tessCoord * (1.0 - tessCoord) + P3 * tessCoord * tessCoord * tessCoord;"+
			"gl_Position = u_mvp_matrix * vec4(P, 1.0);"+
		"}"
	);

	// provide source code to shader
	GLES32.glShaderSource(teasilationEvalutionShaderObject, teasilationEvalutaionShaderSourceCode);

	// compile shader and check for errors
	GLES32.glCompileShader(teasilationEvalutionShaderObject);
	iShaderComipledStatus[0] = 0; // re-initialize
	iInfoLogLength[0] = 0; // re-initialize
	szInfoLog = null; // re-initialize
	GLES32.glGetShaderiv(teasilationEvalutionShaderObject, GLES32.GL_COMPILE_STATUS, iShaderComipledStatus, 0);
	if(iShaderComipledStatus[0] == GLES32.GL_FALSE)
	{
		GLES32.glGetShaderiv(teasilationEvalutionShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
		if(iInfoLogLength[0] > 0)
		{
			szInfoLog = GLES32.glGetShaderInfoLog(teasilationEvalutionShaderObject);
			System.out.println("SSK: Fragment Shader Compilation Log = "+szInfoLog);
			unInitialize();
			System.exit(0);
		}
	}



	///////////////////////////////////////////////////////////////////////////////////////

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
			"out vec4 FragColor;"+
			"uniform vec4 lineColor;"+
			"void main(void)"+
			"{"+
			"FragColor = lineColor;"+
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

		///////////////////////////////////////////////////////////////////////////////////////

		// create shader program
		shaderProgramObject = GLES32.glCreateProgram();

		// attach vertex shader to shader program
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

		GLES32.glAttachShader(shaderProgramObject, teasilationControlShaderObject);

		GLES32.glAttachShader(shaderProgramObject, teasilationEvalutionShaderObject);

		// attach fragment shader to shader program
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

		// pre-link binding of shader program object with vertex shader attributes
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_VERTEX, "vPosition");

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
		gMVPUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		numberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfSegments");
		numberofStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfStrips");
		lineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "lineColor");


		// *** vertices, colors, shader attribs, gVbo_position, gVao initializations ***
		final float triangleVertices[] = new float[]
		{
			-1.0f, -1.0f,
			-0.5f,  1.0f,
			 0.5f, -1.0f,
			 1.0f,  1.0f
		};

		GLES32.glGenVertexArrays(1, gVao, 0);
		GLES32.glBindVertexArray(gVao[0]);

		GLES32.glGenBuffers(1, gVbo_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_position[0]);

		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);	// 4 = sizeof float
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(triangleVertices);
		verticesBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
							triangleVertices.length * 4,
							verticesBuffer,
							GLES32.GL_STATIC_DRAW);

		GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
									2,
									GLES32.GL_FLOAT,
									false, 0, 0);

		GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0); 

		// enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);

		// depth test to do
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		// We will always cull back faces for better performance
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		// Set the background color
		GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// set projectionMatrix to identity matrix
		Matrix.setIdentityM(gPerspectiveProjectionMatrix, 0);

	}

	private void resize(int width, int height)
	{
		// code
		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(gPerspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void draw()
	{
		// Draw background color
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// use shader program
		GLES32.glUseProgram(shaderProgramObject);

		// OpenGL-ES drawing
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];

		// set modelview & modelviewprojection matrices to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -4.0f);

		float lineColor[] = {1.0f, 1.0f, 0.0f, 1.0f};

		modelViewMatrix = translateMatrix;

		// multiply the modelview & projection matrix to get modelviewprojection matrix
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, gPerspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		GLES32.glUniformMatrix4fv(gMVPUniform, 1, false, modelViewProjectionMatrix, 0);
		GLES32.glUniform1i(numberOfSegmentsUniform, uiNumberOfLineSegments);
		GLES32.glUniform1i(numberofStripsUniform, 1);
		GLES32.glUniform4fv(lineColorUniform, 1, lineColor, 0);

		
		// bind gVao
		GLES32.glBindVertexArray(gVao[0]);
		//GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);
		

		// draw, either by glDrawTraingles() or glDrawArrays() or glDrawElements()
		GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4); 	// 3 (each with its x, y, z) vertices in triangleVertices array

		// unbind gVao
		GLES32.glBindVertexArray(0);

		// un-use shader program
		GLES32.glUseProgram(0);

		// render/flush
		requestRender();
	}

	void unInitialize()
	{
		// code
		// destroy gVao
		if(gVao[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, gVao, 0);
			gVao[0] = 0;
		}

		// destroy gVao
		if(gVbo_position[0] != 0)
		{
			GLES32.glDeleteBuffers(1, gVbo_position, 0);
			gVbo_position[0] = 0;
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

