
var canvas = null;
var gl = null;	// webgl context
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

var angle_pyramid = 0.0;
var angle_cube = 0.0;

const WebGLMacros = 
{
	SSK_ATTRIBUTE_POSITION:0,
    SSK_ATTRIBUTE_COLOR:1,
    SSK_ATTRIBUTE_NORMAL:2,
    SSK_ATTRIBUTE_TEXTURE0:3,

};

// Shaders
var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

// Pyramid
var vao_pyramid;
var vbo_Position_pyramid;
var vbo_Position_cube;

// Cube
var vao_cube;
var vbo_Color_cube;
var vbo_Color_pyramid;

var mvpUniform;

var perspeciveProjectionMatrix;


var canvas = document.getElementById('ssk_my_canvas');
var gl = canvas.getContext("webgl2");

// To start animation : To have requestAnimationFrame() to be called "cross-browser" compatible
var requestAnimationFrame = window.requestAnimationFrame		||
							window.webkitRequestAnimationFrame	||
							window.mozRequestAnimationFrame		||
							window.oRequestAnimationFrame		||
							window.msRequestAnimationFrame;


// To stop animation : To have cancelAnimationFrame() to be called
// "cross-browser" compatible
var cancelAnimationFrame = window.cancelAnimationFrame 					||
							window.webkitCancelRequestAnimationFrame	||
							window.webkitCancelAnimationFrame 			||
							window.mozCancelRequestAnimationFrame		||
							window.mozCancelAnimationFrame 				||
							window.oCancelRequestAnimationFrame			||
							window.oCancelAnimationFrame 				||
							window.msCancelRequestAnimationFrame 		||
							window.msCancelAnimationFrame;							

function main()
{
	if(!canvas)
	{
		console.log("Obtaining Canvas failed\n");
	}
	else
	{
		console.log("Obtaining Canvas Succeded\n");
	}

	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;
	

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	init();

	resize();	// warmup REPAINT call due to below draw()

	draw();

}

function toggleFullscreen()
{
	var fullscreen_element 	=	document.fullscreenElement 			||
								document.webkitFullscreenElement	||	// apple sarari browser
								document.mozFullScreenElement		||	// mozila
							 	document.msFullscreenElement		||	// microsoft edge
							 	null;

	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
		{
			canvas.requestFullscreen();
		}
		else if(canvas.webkitRequestFullscreen)
		{
			canvas.webkitRequestFullscreen();
		}
		else if(canvas.mozRequestFullScreen)
		{
			canvas.mozRequestFullScreen();
		}
		else if(canvas.msRequestFullscreen)
		{
			canvas.msRequestFullscreen();
		}

		bFullscreen = true;
	}
	else
	{
		if(document.exitFullscreen)
		{
			document.exitFullscreen();
		}
		else if(document.webkitExitFullscreen)
		{
			document.webkitExitFullscreen();
		}
		else if(document.mozCancelFullScreen)
		{
			document.mozCancelFullScreen();
		}
		else if(document.msExitFullscreen)
		{
			document.msExitFullscreen();
		}

		bFullscreen = false;
	}
}

function keyDown(event)
{
	switch(event.keyCode)
	{
		case 27: // Escape
			uninitialize();
			window.close();	// may not work in Firefox but works in Safari and chrome
			break;

		case 70: 	// F
			toggleFullscreen();
			break;
	}
}

function mouseDown(event)
{

}

function init()
{

	gl = canvas.getContext("webgl2");

	if(!gl)
	{
		console.log("Obtaining webgl2 failed\n");
	}
	else
	{
		console.log("Obtaining webgl2 Succeded\n");
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"in vec4 vColor;"+
	"uniform mat4 u_mvp_matrix;"+
	"out vec4 out_color;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
	"out_color = vColor;"+
	"}";

	gl.shaderSource(vertexShaderObject,vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);

	if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec4 out_color;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = out_color;"+
	"}";

	gl.shaderSource(fragmentShaderObject,fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject,vertexShaderObject);
	gl.attachShader(shaderProgramObject,fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_POSITION,"vPosition");
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_COLOR,"vColor");

	gl.linkProgram(shaderProgramObject);

	if(gl.getProgramParameter(shaderProgramObject,gl.LINK_STATUS) == false)
	{
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	mvpUniform = gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");

	var pyramidVertices = new Float32Array([
												
								//FRONT FACE
								0.0, 0.5, 0.0,
								-0.5,-0.5, 0.5,
								0.5, -0.5,0.5,

								//RIGHT FACE
								0.0, 0.5, 0.0,
								0.5, -0.5, 0.5,
								0.5, -0.5, -0.5,

								//BACK FACE
								0.0, 0.5, 0.0,
								0.5, -0.5,-0.5,
								-0.5, -0.5,-0.5,

								//LEFT FACE
								0.0, 0.5, 0.0,
								-0.5, -0.5,-0.5,
								-0.5, -0.5,0.5,
	]);

	var pyramidColors = new Float32Array([
								//FRONT
								1.0, 0.0, 0.0,
								0.0, 1.0, 0.0,
								0.0, 0.0, 1.0,

								//RIGHT
								1.0, 0.0, 0.0,
								0.0, 0.0, 1.0,
								0.0, 1.0, 0.0,

								//LEFT
								1.0, 0.0, 0.0,
								0.0, 0.0, 1.0,
								0.0, 1.0, 0.0,

								//BACK
								1.0, 0.0, 0.0,
								0.0, 1.0, 0.0,
								0.0, 0.0, 1.0
	]);


    var cubeVertices = new Float32Array([
							//FRONT ACE
							0.5,0.5, 0.5,
							-0.5, 0.5, 0.5,
							-0.5, -0.5, 0.5,
							0.5, -0.5, 0.5,

							//RIGHT FACE
							0.5, 0.5, -0.5,
							0.5, 0.5, 0.5,
							0.5, -0.5, 0.5,
							0.5, -0.5,-0.5,

							//BACK FACE
							0.5, -0.5,-0.5,
							-0.5, -0.5,-0.5,
							-0.5, 0.5, -0.5,
							0.5, 0.5, -0.5,

							//LEFT FACE
							-0.5, 0.5, 0.5,
							-0.5, 0.5, -0.5,
							-0.5, -0.5, -0.5,
							-0.5, -0.5, 0.5,

							//TOP FACE
							0.5, 0.5, -0.5,
							-0.5, 0.5, -0.5,
							-0.5, 0.5, 0.5,
							0.5, 0.5, 0.5,

							//BOTTOM FACE
							0.5, -0.5, 0.5,
							-0.5, -0.5, 0.5,
							-0.5, -0.5, -0.5,
							0.5, -0.5, -0.5
    ]);

	var cubeColors = new Float32Array 
            ([
							//FRONT FACE
							1.0, 0.0, 0.0,
							1.0, 0.0, 0.0,
							1.0, 0.0, 0.0,
							1.0, 0.0, 0.0,

							//RIGHT FACE
							0.0, 0.0, 1.0,
							0.0, 0.0, 1.0,
							0.0, 0.0, 1.0,
							0.0, 0.0, 1.0,

							//BACK FACE
							1.0, 1.0, 0.0,
							1.0, 1.0, 0.0,
							1.0, 1.0, 0.0,
							1.0, 1.0, 0.0,


							//LEFT FACE
							1.0, 0.0, 1.0,
							1.0, 0.0, 1.0,
							1.0, 0.0, 1.0,
							1.0, 0.0, 1.0,

							//TOP FACE
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,
							0.0, 1.0, 0.0,

							//BOTTOM FACE
							1.0, 1.0, 0.0,
							1.0, 1.0, 0.0,
							1.0, 1.0, 0.0,
							1.0, 1.0, 0.0
            ]);

	vao_pyramid = gl.createVertexArray();

	gl.bindVertexArray(vao_pyramid);
		vbo_Position_pyramid = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER,vbo_Position_pyramid);
		gl.bufferData(gl.ARRAY_BUFFER,pyramidVertices,gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,3,gl.FLOAT,false,0,0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER,null);
		
		vbo_Color_pyramid = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER,vbo_Color_pyramid);
		gl.bufferData(gl.ARRAY_BUFFER,pyramidColors,gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,3,gl.FLOAT,false,0,0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);


	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
		vbo_Position_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER,vbo_Position_cube);
		gl.bufferData(gl.ARRAY_BUFFER,cubeVertices,gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,3,gl.FLOAT,false,0,0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER,null);

		vbo_Color_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER,vbo_Color_cube);
		gl.bufferData(gl.ARRAY_BUFFER,cubeColors,gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,3,gl.FLOAT,false,0,0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);

	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	gl.clearColor(0.0,0.0,0.0,1.0);

	perspectiveProjectionMatrix = mat4.create();
}


function resize()
{
	if(bFullscreen == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else
	{
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[-1.5, 0.0, -3.0]);

	mat4.rotateX(modelViewMatrix,modelViewMatrix,degToRad(angle_cube));
	mat4.rotateY(modelViewMatrix,modelViewMatrix,degToRad(angle_cube));
	mat4.rotateZ(modelViewMatrix,modelViewMatrix,degToRad(angle_cube));

	mat4.scale(modelViewMatrix,modelViewMatrix,[0.77, 0.77, 0.77]);

	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);

	gl.bindVertexArray(vao_cube);

	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.drawArrays(gl.TRIANGLE_FAN,4,4);
	gl.drawArrays(gl.TRIANGLE_FAN,8,4);
	gl.drawArrays(gl.TRIANGLE_FAN,12,4);
	gl.drawArrays(gl.TRIANGLE_FAN,16,4);
	gl.drawArrays(gl.TRIANGLE_FAN,20,4);
	
	gl.bindVertexArray(null);
/////////////////////////////////////////////////////////////////////////////////////////////////
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[1.5,0.0,-3.0]);

	mat4.rotateY(modelViewMatrix,modelViewMatrix,degToRad(angle_pyramid));

	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);

	gl.bindVertexArray(vao_pyramid);

	gl.drawArrays(gl.TRIANGLES,0,12);

	gl.bindVertexArray(null);
/////////////////////////////////////////////////////////////////////////////////////////////////
	gl.useProgram(null);

	angle_cube = angle_cube + 1.0;
	if(angle_cube >= 360.0)
	{
		angle_cube = 0.0;
	}

	angle_pyramid = angle_pyramid + 1.0;
	if(angle_pyramid >= 360.0)
	{
		angle_pyramid = 0.0;
	}

	requestAnimationFrame(draw,canvas);
}

function degToRad(degree)
{
	return(degree * Math.PI / 180.0);
}

function uninitialize()
{
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}

	if(vbo_Position_pyramid)
	{
		gl.deleteBuffer(vbo_Position_pyramid);
		vbo_Position_pyramid = null;
	}

	if(vbo_Color_pyramid)
	{
		gl.deleteBuffer(vbo_Color_pyramid);
		vbo_Color_pyramid = null;
	}

	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube= null;
	}

	if(vbo_Position_cube)
	{
		gl.deleteBuffer(vbo_Position_cube);
		vbo_Position_cube = null;
	}

	if(vbo_Color_cube)
	{
		gl.deleteBuffer(vbo_Color_cube);
		vbo_Color_cube = null;
	}
	
	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject,fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}

		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject,vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
	
}