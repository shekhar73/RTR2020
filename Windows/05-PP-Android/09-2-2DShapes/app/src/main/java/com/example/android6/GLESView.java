package com.example.android6;

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

	private int[] vao_triangle = new int[1];
	private int[] vbo_triangle = new int[1];
	private int[] vbo_triangle_color = new int[1];

	private int[] vao_square = new int[1];
	private int[] vbo_square = new int[1];
	private int[] vbo_square_color = new int[1];

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
		int[] iShaderCompiledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
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
		iShaderCompiledStatus[0] = 0; // re-initialize
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize
		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
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
		GLES32.glGetShaderiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderCompiledStatus, 0);
		if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
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
		final float triangleVertices[] = new float[]
		{
			 0.0f, 	  1.0f,  0.0f, 		// appex
			-1.0f,  -1.0f,  0.0f,			// left-bottom
			 1.0f,  -1.0f,  0.0f			// right-bottom
		};

		final float colorTriangle[] = new float[]
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		final float squareVertices[] = new float[]
		{
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f
		};

		final float colorSquare[] = new float[]
		{	0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 
			0.0f, 0.0f, 1.0f,
		};

		GLES32.glGenVertexArrays(1, vao_triangle, 0);
		GLES32.glBindVertexArray(vao_triangle[0]);


		// Triangle
		GLES32.glGenBuffers(1, vbo_triangle, 0);
		{
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_triangle[0]);

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
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		}
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


		// Triangle Color
		GLES32.glGenBuffers(1, vbo_triangle_color, 0);
		{
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_triangle_color[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(colorTriangle.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(colorTriangle);
			colorBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
				colorTriangle.length * 4,
				colorBuffer,
				GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_COLOR,
				3,
				GLES32.GL_FLOAT,
				false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_COLOR);
		
		}
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);




		

		//Square
		GLES32.glGenVertexArrays(1, vao_square, 0);
        GLES32.glBindVertexArray(vao_square[0]);
        GLES32.glGenBuffers(1, vbo_square, 0);
		{
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_square[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(squareVertices);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
				squareVertices.length * 4,
				verticesBuffer,
				GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
				3,
				GLES32.GL_FLOAT,
				false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);
		}
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		// Square Color
		GLES32.glGenBuffers(1, vbo_square_color, 0);
		{
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_square_color[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(colorSquare.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(colorSquare);
			colorBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
				colorSquare.length * 4,
				colorBuffer,
				GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_COLOR,
				3,
				GLES32.GL_FLOAT,
				false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_COLOR);
		
		}
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glVertexAttrib3f(GLESMacros.SSK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
		GLES32.glBindVertexArray(0);


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
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];

		///////////////////////////////////////////////////////////////////////////////
		// Draw Triangle
		///////////////////////////////////////////////////////////////////////////////
		// set modelview & modelviewprojection matrices to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, 1.5f, 0.0f, -6.0f);
		modelViewMatrix = translateMatrix;

		// multiply the modelview & projection matrix to get modelviewprojection matrix
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);


		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		GLES32.glBindVertexArray(vao_triangle[0]);
		{
			GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);
		}
		GLES32.glBindVertexArray(0);

		///////////////////////////////////////////////////////////////////////////////
		// Draw Square
		///////////////////////////////////////////////////////////////////////////////
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, -1.5f, 0.0f, -6.0f);
		modelViewMatrix = translateMatrix;


		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		GLES32.glBindVertexArray(vao_square[0]);
		{
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		}
		

		// unbind vao
		GLES32.glBindVertexArray(0);

		// un-use shader program
		GLES32.glUseProgram(0);

		// render/flush
		requestRender();
	}

	void unInitialize()
	{
		// code
		// destroy vao_triangle
		if(vao_triangle[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
			vao_triangle[0] = 0;
		}

		// destroy vbo_triangle
		if(vbo_triangle[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_triangle, 0);
			vbo_triangle[0] = 0;
		}


		// destroy vao
		if(vbo_triangle_color[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_triangle_color, 0);
			vbo_triangle_color[0] = 0;
		}

		if(vao_square[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_square, 0);
			vao_square[0] = 0;
		}

		if(vbo_square[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vbo_square, 0);
			vbo_square[0] = 0;
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

