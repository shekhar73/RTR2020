package com.example.ThreeDShapesTexture;

import android.content.Context;		// for drawing context related
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.opengl.GLSurfaceView;	// for OpenGL Surface View and all related
import android.opengl.GLES32;	// for OpenGLES 3.2
import javax.microedition.khronos.opengles.GL10;	// for OpenGLES 1.0 needed as param type EGLConfig
import javax.microedition.khronos.egl.EGLConfig;	// for EGLConfig needed as param type EGLConfig

import android.graphics.BitmapFactory;
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

	private int[] vao_cube = new int[1];
	private int[] vbo_position_cube = new int[1];
	private int[] vbo_texture_cube = new int[1];

	private int[] vao_pyramid = new int[1];
	private int[] vbo_position_pyramid = new int[1];
	private int[] vbo_texture_pyramid = new int[1];

	private int Stone_Texture;
	private int Kundali_Texture;

	private int textureSamplerUniform;

	float angleCube = 0.0f;
	float anglepyramid = 0.0f;

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
		System.out.println("SSK : Double Tap.");
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
			"FragColor = texture(u_texture_sampler, out_TexCoord);"+
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SSK_ATTRIBUTE_TEXCOORD0, "vTexCoord");

		// link the two shaders together to shader program object
		GLES32.glLinkProgram(shaderProgramObject);

		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0; // re-initialize
		szInfoLog = null; // re-initialize

		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
		
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
		textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

		// *** vertices, colors, shader attribs, vbo, vao initializations ***
		final float cubeVerties[] = new float[]
		{
				//front
		         1.0f,	1.0f,	1.0f,
		        -1.0f,	1.0f,	1.0f,
		        -1.0f,	-1.0f,	1.0f,
		        1.0f,	-1.0f,	1.0f,

		        //right
		        1.0f,	1.0f,	-1.0f,
		        1.0f,	1.0f,	1.0f,
		        1.0f,	-1.0f,	1.0f,
		        1.0f,	-1.0f,	-1.0f,

		        //back
		        -1.0f,	1.0f,	-1.0f,
		        1.0f,	1.0f,	-1.0f,
		        1.0f,  -1.0f,	-1.0f,
		        -1.0f, -1.0f,	-1.0f,

		        //left
		        -1.0f,	1.0f, 	1.0f,
		        -1.0f, 	1.0f,  -1.0f,
		        -1.0f, -1.0f,  -1.0f,
		        -1.0f, -1.0f, 	1.0f,

		        //top
		        1.0f,	1.0f,	-1.0f,
		        -1.0f,	1.0f,	-1.0f,
		        -1.0f,	1.0f,	1.0f,
		        1.0f,	1.0f,	1.0f,

		        //bottom
		        1.0f,	-1.0f,	-1.0f,
		        -1.0f,	-1.0f,	-1.0f,
		        -1.0f,	-1.0f,	1.0f,
		        1.0f,	-1.0f,	1.0f
	
		};

		final float cubeTexCoord[] = new float[]
        {
				0.0f, 0.0f,
		        1.0f, 0.0f,
		        1.0f, 1.0f,
		        0.0f, 1.0f,


		        0.0f, 0.0f,
		        1.0f, 0.0f,
		        1.0f, 1.0f,
		        0.0f, 1.0f,
	
		        1.0f, 0.0f,
		        1.0f, 1.0f,
		        0.0f, 1.0f,
		        0.0f, 0.0f,

		        0.0f, 0.0f,
		        1.0f, 0.0f,
		        1.0f, 1.0f,
		        0.0f, 1.0f,

		        0.0f, 0.0f,
		        1.0f, 0.0f,
		        1.0f, 1.0f,
		        0.0f, 1.0f,


		        0.0f, 0.0f,
		        1.0f, 0.0f,
		        1.0f, 1.0f,
		        0.0f, 1.0f,

        };


		final float pyramidVerties[] = new float[]
		{
				//front
		        0.0f,	1.0f,	0.0f,	//apex
		        -1.0f,	-1.0f,	1.0f,	//left bottom
		        1.0f,	-1.0f,	1.0f,	//right bottom

		        //right
		        0.0f,	1.0f,	0.0f,
		        1.0f,	-1.0f,	1.0f,
		        1.0f,	-1.0f,	-1.0f,

		        //back
		        0.0f,	1.0f,	0.0f,
		        1.0f,	-1.0f,	-1.0f,
		        -1.0f,	-1.0f,	-1.0f,

		        //left
		        0.0f,	1.0f,	0.0f,
		        -1.0f,	-1.0f,	-1.0f,
		        -1.0f,	-1.0f,	1.0f
	
		};


         final float pyramidTexCoord[] = new float[]
         {
			 	0.5f, 1.0f,
		        0.0f, 0.0f,
		        1.0f, 0.0f,


		        0.5f, 1.0f,
		        1.0f, 0.0f,
		        0.0f, 0.0f,


		        0.5f, 1.0f,
		        0.0f, 0.0f,
		        1.0f, 0.0f,


		        0.5f, 1.0f,
		        0.0f, 0.0f,
		        1.0f, 0.0f

         };
		
		

		//------------------------------------Cube--------------------------------
		GLES32.glGenVertexArrays(1, vao_cube, 0);
		GLES32.glBindVertexArray(vao_cube[0]);

		GLES32.glGenBuffers(1, vbo_position_cube, 0);
		
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cubeVerties.length * 4);	// 4 = sizeof float
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(cubeVerties);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
								cubeVerties.length * 4,
								verticesBuffer,
								GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//-------------------------------------------------------------------------------
		// CUBE TEXTURE
		//-------------------------------------------------------------------------------

		GLES32.glGenBuffers(1, vbo_texture_cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture_cube[0]);

		byteBuffer = ByteBuffer.allocateDirect(cubeTexCoord.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer colorBufferCube = byteBuffer.asFloatBuffer();
		colorBufferCube.put(cubeTexCoord);
		colorBufferCube.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
			cubeTexCoord.length * 4,
			colorBufferCube,
			GLES32.GL_STATIC_DRAW);

		GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_TEXCOORD0,
			2,
			GLES32.GL_FLOAT,
			false, 0, 0);

		GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_TEXCOORD0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
		//-------------------------------------------------------------------------------
		

		GLES32.glGenVertexArrays(1, vao_pyramid, 0);
		GLES32.glBindVertexArray(vao_pyramid[0]);


		// Pyramid
		GLES32.glGenBuffers(1, vbo_position_pyramid, 0);
		
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_pyramid[0]);

			byteBuffer = ByteBuffer.allocateDirect(pyramidVerties.length * 4);	// 4 = sizeof float
			byteBuffer.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(pyramidVerties);
			verticesBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
								pyramidVerties.length * 4,
								verticesBuffer,
								GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
										3,
										GLES32.GL_FLOAT,
										false, 0, 0);

			GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//-------------------------------------------------------------------------------
		// PYRAMID TEXTURE
		//-------------------------------------------------------------------------------
		
		GLES32.glGenBuffers(1, vbo_texture_pyramid, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture_pyramid[0]);

		byteBuffer = ByteBuffer.allocateDirect(pyramidTexCoord.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer colorBufferPyramid = byteBuffer.asFloatBuffer();
		colorBufferPyramid.put(pyramidTexCoord);
		colorBufferPyramid.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
			pyramidTexCoord.length * 4,
			colorBufferPyramid,
			GLES32.GL_STATIC_DRAW);

		GLES32.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_TEXCOORD0,
			2,
			GLES32.GL_FLOAT,
			false, 0, 0);

		GLES32.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_TEXCOORD0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		//-------------------------------------------------------------------------------

		// enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);

		// depth test to do
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// We will always cull back faces for better performance
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		Stone_Texture = LoadGLTexture(R.raw.stone);			// R: Resource, 
		Kundali_Texture = LoadGLTexture(R.raw.kundali);

		// Set the background color
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// set projectionMatrix to identity matrix
		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	private int LoadGLTexture(int imageFileResourceID)
	{
		// constructor
		BitmapFactory.Options options = new BitmapFactory.Options();

		options.inScaled = false;

		// OS Bitmap loading
		Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);

		int texture[] = new int[1];

		// OpenGL Code
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
		GLES32.glGenTextures(1, texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

		// setting texture parameter
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

		// following call will actualy push the data into graphic memory card
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);

		return (texture[0]);
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
		// Draw background color
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// use shader program
		GLES32.glUseProgram(shaderProgramObject);

		// OpenGL-ES drawing
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];

		///////////////////////////////////////////////////////////////////////////////
		// Draw Cube
		///////////////////////////////////////////////////////////////////////////////
		// set modelview & modelviewprojection matrices to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(modelViewMatrix, 0, -2.0f, 0.0f, -6.0f);
		Matrix.rotateM(modelViewMatrix, 0, angleCube, 1.0f, 1.0f, 1.0f);

		// multiply the modelview & projection matrix to get modelviewprojection matrix
		//Matrix.multiplyMM(modelViewMatrix, 0, translateMatrix, 0, rotationMatrix, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);


		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, Kundali_Texture);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_cube[0]);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);
		// unbind vao
		GLES32.glBindVertexArray(0);



		///////////////////////////////////////////////////////////////////////////////
		// Draw Cube
		///////////////////////////////////////////////////////////////////////////////
		// set modelview & modelviewprojection matrices to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(modelViewMatrix, 0, 2.0f, 0.0f, -6.0f);
		Matrix.rotateM(modelViewMatrix, 0, anglepyramid, 0.0f, 1.0f, 0.0f);


		// multiply the modelview & projection matrix to get modelviewprojection matrix
		///Matrix.multiplyMM(modelViewMatrix, 0, translateMatrix, 0, rotationMatrix, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);


		// pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
		// whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, Stone_Texture);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_pyramid[0]);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);
		// unbind vao
		GLES32.glBindVertexArray(0);

		// un-use shader program
		GLES32.glUseProgram(0);


		anglepyramid = anglepyramid + 0.5f;
		if(anglepyramid >= 360.0f)
		{
			anglepyramid = 0.0f;
		}


		angleCube = angleCube + 0.5f;
		if(angleCube >= 360.0f)
		{
			angleCube = 0.0f;
		}

		// render/flush
		requestRender();
	}

	void unInitialize()
	{
		// code
		
		// pyramid
		if(vao_pyramid[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_pyramid, 0);
			vao_pyramid[0] = 0;
		}


		if(vbo_position_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_pyramid, 0);
			vbo_position_pyramid[0] = 0;
		}


		if(vbo_texture_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texture_pyramid, 0);
			vbo_texture_pyramid[0] = 0;
		}

		// cube
		if(vao_cube[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_cube, 0);
			vao_cube[0] = 0;
		}
		
		if(vbo_position_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_cube, 0);
			vbo_position_cube[0] = 0;
		}

		if(vbo_texture_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texture_cube, 0);
			vbo_texture_cube[0] = 0;
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
