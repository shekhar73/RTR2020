// global variables

var canvas = null;
var gl = null;	// webgl context
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros = {	// when whole 'WebGLMacros' is 'const', all inside it are automatically 'const'
	SSK_ATTRIBUTE_POSITION:0,
	SSK_ATTRIBUTE_COLOR:1,
	SSK_ATTRIBUTE_NORMAL:2,
	SSK_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_triangle;
var vbo_position_triangle;
var vbo_color_triangle;

var vao_square;
var vbo_position_square;
var vbo_color_square;

var mvpUniform;

var perspectiveProjectionMatrix;

var angleTriangle = 0.0;
var angleRectangle = 0.0;

var canvas = document.getElementById('ssk_my_canvas');
var gl = canvas.getContext("webgl2");

var bFullscreen = false;

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

	canvas_orginal_width = canvas.width;
	canvas_orginal_height = canvas.height;
	

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	init();

	resize();	// warmpu REPAINT call due to below draw()

	draw();

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

function toggleFullscreen()
{
	var fullscreen_element 	=	document.fullscreenElement 			||
								document.webkitfullscreenElement	||	// apple sarari browser
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

function mouseDown(event)
{

}

function init()
{

	if(!gl)
	{
		console.log("Obtaining webgl2 failed\n");
	}
	else
	{
		console.log("Obtaining webgl2 Succeded\n");
	}

	gl.viewPortwidth = canvas.width;
	gl.viewPortHeight = canvas.height;


	// vertex shader

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	var vertesShaderSourceCode = 
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

	gl.shaderSource(vertexShaderObject, vertesShaderSourceCode);
	gl.compileShader(vertexShaderObject);

	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// fragment shader
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

	

	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);

	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// shader program
	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	//pre-link binding of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_COLOR, "vColor");

	// linking
	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false)
	{
	 	var error = gl.getProgramInfoLog(shaderProgramObject);
	 	if(error.length > 0)
	 	{
	 		alert(error);
	 		uninitialize();
	 	}
	}

	// get MVP uniform location
	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");

	// *** vertices, colors, shader attribs, vbo, vao initializations *** 
	var triangleVertices = new Float32Array([
	 											0.0, 0.5, 0.0,		// appex
	 											-0.5, -0.5, 0.0, 	// left-bottom
	 											0.5, -0.5, 0.0 		// right-bottom
	 	]);

	var colorVertices = new Float32Array([
	 										1.0, 0.0, 0.0,
	 										0.0, 1.0, 0.0,
	 										0.0, 0.0, 1.0
	 	]);

	var rectangleVertices = new Float32Array([
	 											0.5, 0.5, 0.0,
	 											-0.5, 0.5, 0.0,
	 											-0.5, -0.5, 0.0,
	 											0.5, -0.5, 0.0
	 	]);

	vao_triangle = gl.createVertexArray();
	gl.bindVertexArray(vao_triangle);
	{
		vbo_position_triangle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_triangle);
		gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vbo_color_triangle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_triangle);
		gl.bufferData(gl.ARRAY_BUFFER, colorVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR ,3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	}
	gl.bindVertexArray(null);

	vao_square = gl.createVertexArray();
	gl.bindVertexArray(vao_square);
	{
		vbo_position_square = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_square);
		gl.bufferData(gl.ARRAY_BUFFER, rectangleVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		gl.vertexAttrib3f(WebGLMacros.SSK_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
	}
	
	gl.bindVertexArray(null);

	gl.clearColor(0.0, 0.0, 0.0, 1.0);

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
		canvas.width = canvas_orginal_width;
		canvas.height = canvas_orginal_height;
	}

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	////////////////////////////////////////////////////////////////////////////////////////////
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	var rotationMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -3.0]);
	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angleRectangle));
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
	gl.bindVertexArray(vao_square);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.bindVertexArray(null);

	//////////////////////////////////////////////////////////////////////////////////////////////

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -3.0]);
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angleTriangle));
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
	gl.bindVertexArray(vao_triangle);
	gl.drawArrays(gl.TRIANGLES, 0, 3);
	gl.bindVertexArray(null);
	////////////////////////////////////////////////////////////////////////////////////////////////

	gl.useProgram(null);

	angleRectangle = angleRectangle + 1.0;
	if(angleRectangle >= 360.0)
	{
		angleRectangle = 0.0;
	}

	angleTriangle = angleTriangle + 1.0;
	if(angleTriangle >= 360.0)
	{
		angleTriangle = 0.0;
	}

	requestAnimationFrame(draw, canvas);

}

function degToRad(degree)
{
	return (degree * Math.PI / 180.0);
}

function uninitialize()
{
	// code
	if(vao_triangle)
	{
		gl.deleteVertexArray(vao_triangle);
		vao_triangle = null;
	}

	if(vbo_position_triangle)
	{
		gl.deleteVertexArray(vbo_position_triangle);
		vbo_position_triangle = null;
	}

	if(vbo_color_triangle)
	{
		gl.deleteBuffer(vbo_color_triangle);
		vbo_color_triangle = null;
	}

	if(vao_square)
	{
		gl.deleteVertexArray(vao_square);
		vao_square = null;
	}

	if(vbo_position_square)
	{
		gl.deleteVertexArray(vbo_position_square);
		vbo_position_square = null;
	}

	if(vbo_color_square)
	{
		gl.deleteBuffer(vbo_color_square);
		vbo_color_square = null;
	}



	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}

		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
}