package com.example.graphpaper;

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

	private int[] gVao_Graph = new int[1];
	private int[] gVbo_Graph_Vertex = new int[1];
	private int[] gVbo_Graph_Color = new int[1];

	private int[] gVao_Axis = new int[1];
	private int[] gVbo_Axis_Vertex = new int[1];
	private int[] gVbo_Axis_Color = new int[1];

	private int mvpUniform;

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
			"in vec4 vColor;"+
			"uniform mat4 u_mvp_matrix;"+
			"out vec4 out_color;"+
			"void main(void)"+
			"{"+
				"gl_Position = u_mvp_matrix * vPosition;"+
				"out_color = vColor;"+
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
			"in vec4 out_color;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
			"FragColor = out_color;"+
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_VERTEX, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_COLOR, "vColor");

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
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");

		// *** vertices, colors, shader attribs, vbo, vao initializations ***
	//---------------------GRAPH---------------------------------------------------------
		final float graphVertices[] = new float[42 * 2 * 3 * 2];

		float x = -1.0f;
		for(int i = 0; i < 41 * 2 * 3; i = i + 6)
		{
			graphVertices[i] = x;
			graphVertices[i + 1] = 1.0f;
			graphVertices[i + 2] = 0.0f;

			graphVertices[i + 3] = x;
			graphVertices[i + 4] = -1.0f;
			graphVertices[i + 5] = 0.0f;

			x = x + 0.05f;
		}

		float y = 1.0f;
		for(int i = 0; i < 41 * 2 * 3; i = i + 6)
		{
			graphVertices[i] = -1.0f;
			graphVertices[i + 1] = y;
			graphVertices[i + 2] = 0.0f;

			graphVertices[i + 3] = 1.0f;
			graphVertices[i + 4] = y;
			graphVertices[i + 5] = 0.0f;

			y = y - 0.05f;
		}

		float graphColors[] = new float [42 * 2 * 3 * 2];
		for(int i = 0; i < 42 * 2 * 3 * 2; i = i + 3)
		{
			graphColors[i] = 0.0f;
			graphColors[i + 1] = 0.0f;
			graphColors[i + 2] = 1.0f;
		}

		/*final float triangleVertices[] = new float[]
		{
			 0.0f, 	  1.0f,  0.0f, 		// appex
			-1.0f,  -1.0f,  0.0f,			// left-bottom
			 1.0f,  -1.0f,  0.0f			// right-bottom
		};*/

		GLES32.glGenVertexArrays(1, gVao_Graph, 0);
			GLES32.glBindVertexArray(gVao_Graph[0]);

			GLES32.glGenBuffers(1, gVbo_Graph_Vertex, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_Graph_Vertex[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(graphVertices.length * 4);	// 4 = sizeof float
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(graphVertices);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
								graphVertices.length * 4,
								verticesBuffer,
								GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);



			GLES32.glGenBuffers(1, gVbo_Graph_Color, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_Graph_Color[0]);

			byteBuffer = ByteBuffer.allocateDirect(graphColors.length * 4);	// 4 = sizeof float
			byteBuffer.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(graphColors);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
								graphColors.length * 4,
								verticesBuffer,
								GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_COLOR,
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_COLOR);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0); 



//---------------------End of GRAPH---------------------------------------------------------


//----------------- AXIS of X and Y---------------------------------------------------------
	

	final float axisVertices[] = new float[]
	{
		-1.0f,	0.0f,	0.0f,
		 1.0f,	0.0f,	0.0f,
		 0.0f,	1.0f,	0.0f,
		 0.0f, -1.0f,	0.0f
	};

	final float axisColors[] = new float[]
	{
		1.0f,	0.0f,	0.0f,
		1.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,
		0.0f,	1.0f,	0.0f,
	};

	GLES32.glGenVertexArrays(1, gVao_Axis, 0);
			GLES32.glBindVertexArray(gVao_Axis[0]);

			GLES32.glGenBuffers(1, gVbo_Axis_Vertex, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_Axis_Vertex[0]);

			byteBuffer = ByteBuffer.allocateDirect(axisVertices.length * 4);	// 4 = sizeof float
			byteBuffer.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(axisVertices);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
								axisVertices.length * 4,
								verticesBuffer,
								GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);



			GLES32.glGenBuffers(1, gVbo_Axis_Color, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, gVbo_Axis_Color[0]);

			byteBuffer = ByteBuffer.allocateDirect(axisColors.length * 4);	// 4 = sizeof float
			byteBuffer.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(axisColors);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
								axisColors.length * 4,
								verticesBuffer,
								GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_COLOR,
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_COLOR);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0); 

//----------------- End of AXIS of X and Y-----------------------------------------------



		// enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);

		// depth test to do
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		// We will always cull back faces for better performance
		GLES32.glEnable(GLES32.GL_CULL_FACE);

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

		// OpenGL-ES drawing
		float translateMatrix[] = new float[16];
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];
		

		// set modelview & modelviewprojection matrices to identity
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);

		//--------------------- Graph------------------------------------------------------
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		modelViewMatrix = translateMatrix;

		// multiply the modelview & projection matrix to get modelviewprojection matrix
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		// bind vao
		GLES32.glBindVertexArray(gVao_Graph[0]);

		// 41 lines in x direction and 41 lines in Y direction
		for(int i = 0; i < 41 * 2; i++)
		{
			GLES32.glDrawArrays(GLES32.GL_LINES, i * 2, 2);
		}
		GLES32.glBindVertexArray(0);
		//------------------ End of graph----------------------------------------------------

		//---------------------- Draw Axises------------------------------------------------
		GLES32.glBindVertexArray(gVao_Axis[0]);
			// X-axis
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			// Y-axis
			GLES32.glDrawArrays(GLES32.GL_LINES, 2, 2);
		GLES32.glBindVertexArray(0);
		//--------------------------------- End of axises-------------------------------------

		// un-use shader program
		GLES32.glUseProgram(0);

		// render/flush
		requestRender();
	}

	void unInitialize()
	{
		// code
		// destroy vao
		if(gVao_Graph[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, gVao_Graph, 0);
			gVao_Graph[0] = 0;
		}

		// destroy vao
		if(gVbo_Graph_Vertex[0] != 0)
		{
			GLES32.glDeleteBuffers(1, gVbo_Graph_Vertex, 0);
			gVbo_Graph_Vertex[0] = 0;
		}

		// destroy vao
		if(gVao_Axis[0] != 0)
		{
			GLES32.glDeleteBuffers(1, gVao_Axis, 0);
			gVao_Axis[0] = 0;
		}

		// destroy vao
		if(gVbo_Axis_Vertex[0] != 0)
		{
			GLES32.glDeleteBuffers(1, gVbo_Axis_Vertex, 0);
			gVbo_Axis_Vertex[0] = 0;
		}

		// destroy vao
		if(gVbo_Graph_Color[0] != 0)
		{
			GLES32.glDeleteBuffers(1, gVbo_Graph_Color, 0);
			gVbo_Graph_Color[0] = 0;
		}

		// destroy vao
		if(gVbo_Axis_Color[0] != 0)
		{
			GLES32.glDeleteBuffers(1, gVbo_Axis_Color, 0);
			gVbo_Axis_Color[0] = 0;
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

