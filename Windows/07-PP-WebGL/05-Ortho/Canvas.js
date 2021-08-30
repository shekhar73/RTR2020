// global variables

var canvas = null;
var gl = null;	// webgl context
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

const webGLMacros = {	// when whole 'WebGLMacros' is 'const', all inside it are automatically 'const'
	SSK_ATTRIBUTE_POSITION:0,
	SSK_ATTRIBUTE_COLOR:1,
	SSK_ATTRIBUTE_NORMAL:2,
	SSK_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao;
var vbo;
var mvpUniform;

var orthographicProjectionMatrix;

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
	

	window.addEventListener("resize", resize, false);
	window.addEventListener("keydown", keyDown, false);

	init();

	resize();	// warmpu REPAINT call due to below draw()

	draw();

}

function keyDown()
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

function mouseDown()
{
	
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
		else if(canvas.mozFRequestFullScreen)
		{
			canvas.mozFRequestFullScreen();
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
	var vertesShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"uniform mat4 u_mvp_matrix;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
	"}";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertesShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// fragment shader
	var fragmentShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = vec4(1.0, 1.0, 1.0, 1.0);"+
	"}"

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
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
	 gl.bindAttribLocation(shaderProgramObject, webGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");

	 // linking
	 gl.linkProgram(shaderProgramObject);
	 if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
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
	 											0.0, 50.0, 0.0,		// appex
	 											-50.0, -50.0, 0.0, 	// left-bottom
	 											50.0, -50.0, 0.0 	// right-bottom
	 	]);

	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	vbo = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
	gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(webGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(webGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.clearColor(0.0, 0.0, 1.0, 1.0);

	orthographicProjectionMatrix = mat4.create();
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

	// Orthographic Projection => left, right, bottom, top, near, far
	if(canvas.width <= canvas.height)
		mat4.ortho(orthographicProjectionMatrix, -100.0, 100.0, (-100.0 * (canvas.height / canvas.width)), (100.0 * (canvas.height / canvas.width)), -100.0, 100.0);
	else
		mat4.ortho(orthographicProjectionMatrix, (-100.0 * (canvas.width / canvas.height)), (100.0 * (canvas.width / canvas.height)), -100.0, 100.0, -100.0, 100.0);

}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrx = mat4.create();
	mat4.multiply(modelViewProjectionMatrx, orthographicProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrx);
	gl.bindVertexArray(vao);

	gl.drawArrays(gl.TRIANGLES, 0, 3);

	gl.bindVertexArray(null);

	gl.useProgram(null);

	requestAnimationFrame(draw, canvas);

}

function uninitialize()
{
	// code
	if(vao)
	{
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if(vbo)
	{
		gl.deleteVertexArray(vbo);
		vbo = null;
	}

	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			vertexShaderObject = null;
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