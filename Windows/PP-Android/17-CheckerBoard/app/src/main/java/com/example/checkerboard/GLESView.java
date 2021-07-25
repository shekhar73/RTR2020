package com.example.checkerboard;

import android.content.Context;		// for drawing context related
import android.opengl.GLSurfaceView;	// for OpenGL Surface View and all related
import javax.microedition.khronos.opengles.GL10;	// for OpenGLES 1.0 needed as param type EGLConfig
import javax.microedition.khronos.egl.EGLConfig;	// for EGLConfig needed as param type EGLConfig

import android.opengl.GLES32;	// for OpenGLES 3.2
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.graphics.Bitmap;
import android.opengl.GLUtils;

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

	private int checkerBoardWidth = 64;
	private int checkerBoardHeight = 64;
	private byte[] CheckImage = new byte[checkerBoardWidth * checkerBoardHeight * 4];

	private int[] vao = new int[1];
	private int[] vbo_position_checker = new int[1];
	private int[] vbo_texture_checker = new int[1];

	private float[] verticesStraight = new float[12];
	private float[] verticesAngular = new float[12];

	private int[] TextureID = new int[1];

	private int mvpUniform;
	private int textureSamplerUniform;


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
			"in vec2 vTexCoord;"+
			"uniform mat4 u_mvp_matrix;"+
			"out vec2 out_TexCoord;"+
			"void main(void)"+
			"{"+
				"gl_Position = u_mvp_matrix * vPosition;"+
				"out_TexCoord = vTexCoord;"+
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
			"in vec2 out_TexCoord;"+
			"uniform highp sampler2D u_texture_sampler;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
			"	FragColor = texture(u_texture_sampler, out_TexCoord);"+
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_TEXCOORD0, "vTexCoord");

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
		textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

		// *** vertices, colors, shader attribs, vbo, vao initializations ***
		final float checkerBoardTexCoord[] = new float[]
		{
			1.0f,	1.0f,
			0.0f,	1.0f,
			0.0f,	0.0f,
			1.0f,	0.0f
		};

		GLES32.glGenVertexArrays(1, vao, 0);
			GLES32.glBindVertexArray(vao[0]);
				GLES32.glGenBuffers(1, vbo_position_checker, 0);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_checker[0]);
					GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);
					GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
					GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_POSITION);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				GLES32.glGenBuffers(1, vbo_texture_checker, 0);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture_checker[0]);
					
					ByteBuffer byteBuffer = ByteBuffer.allocateDirect(checkerBoardTexCoord.length * 4);
					byteBuffer.order(ByteOrder.nativeOrder());
					FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();
					colorBuffer.put(checkerBoardTexCoord);
					colorBuffer.position(0);
					
					GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, checkerBoardTexCoord.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
					GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0, 0);
					GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_TEXCOORD0);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		// enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);

		// depth test to do
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		// We will always cull back faces for better performance

		TextureID[0] = LoadGLTexture();
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);


		// Set the background color
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// set projectionMatrix to identity matrix
		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	private int LoadGLTexture()
	{
		int TEXT_IMAGE[] = new int[1];
	    Bitmap bitmap = Bitmap.createBitmap(checkerBoardWidth, checkerBoardHeight, Bitmap.Config.ARGB_8888);
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(checkerBoardWidth * checkerBoardHeight * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		MakeCheckImage();
		byteBuffer.put(CheckImage);
		byteBuffer.position(0);
		bitmap.copyPixelsFromBuffer(byteBuffer);

		GLES32.glGenTextures(1,TEXT_IMAGE, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, TEXT_IMAGE[0]);
		
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT); 
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

		return(TEXT_IMAGE[0]);
		
	}

	private void MakeCheckImage()
	{
		int i, j, c;
		for(i = 0; i < checkerBoardWidth; i++)
		{
			for(j = 0; j < checkerBoardHeight; j++)
			{
				c = ((i & 8) ^ (j & 8)) * 255;
				CheckImage[(i * 64 + j) * 4 + 0] = (byte)c;
				CheckImage[(i * 64 + j) * 4 + 1] = (byte)c;
				CheckImage[(i * 64 + j) * 4 + 2] = (byte)c;
				CheckImage[(i * 64 + j) * 4 + 3] = (byte)0XFF;
			}
		}
	}

	private void resize(int width,int height)
	{
		if(height == 0)
		{
			height = 1;
		}		
		GLES32.glViewport(0,0,width,height);	
		Matrix.perspectiveM(perspectiveProjectionMatrix , 0 , 45.0f , ((float)width / (float)height) , 0.1f , 100.0f);
	}

	private void draw()
	{
		// Draw background color
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// use shader program
		GLES32.glUseProgram(shaderProgramObject);

		// OpenGL-ES drawing
		float modelViewMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];

		// set modelview & modelviewprojection matrices to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.0f);

		// multiply the modelview & projection matrix to get modelviewprojection matrix
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, TextureID[0]);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		///////////////////////////////////////////////////////////////////////////////////////

		GLES32.glBindVertexArray(vao[0]);
			verticesStraight[0] = -2.0f;
			verticesStraight[1] = -1.0f;
			verticesStraight[2] =  0.0f;
			verticesStraight[3] = -2.0f;
			verticesStraight[4] =  1.0f;
			verticesStraight[5] =  0.0f;
			verticesStraight[6] =  0.0f;
			verticesStraight[7] =  1.0f;
			verticesStraight[8] =  0.0f;
			verticesStraight[9] =  0.0f;
			verticesStraight[10] = -1.0f;
			verticesStraight[11] = 0.0f;

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_checker[0]);
				
				ByteBuffer byteBuffer = ByteBuffer.allocateDirect(verticesStraight.length * 4);
				byteBuffer.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
				verticesBuffer.put(verticesStraight);
				verticesBuffer.position(0);
				
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, verticesStraight.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glBindVertexArray(0);

		//////////////////////////////////////////////////////////////////////////////////////

		GLES32.glBindVertexArray(vao[0]);
			verticesAngular[0] =  1.0f;
			verticesAngular[1] = -1.0f;
			verticesAngular[2] =  0.0f;
			verticesAngular[3] =  1.0f;
			verticesAngular[4] =  1.0f;
			verticesAngular[5] =  0.0f;
			verticesAngular[6] =  2.41421f;
			verticesAngular[7] =  1.0f;
			verticesAngular[8] = -1.41421f;
			verticesAngular[9] =  2.41421f;
			verticesAngular[10] = -1.0f;
			verticesAngular[11] = -1.41421f;


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_checker[0]);
			
			byteBuffer = ByteBuffer.allocateDirect(verticesAngular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(verticesAngular);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, verticesAngular.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glBindVertexArray(0);

		// un-use shader program
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

		if(vbo_position_checker[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_checker, 0);
			vbo_position_checker[0] = 0;
		}

		if(vbo_texture_checker[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texture_checker, 0);
			vbo_texture_checker[0] = 0;
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

