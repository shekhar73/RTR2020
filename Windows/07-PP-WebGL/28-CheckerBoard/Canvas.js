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

var gVao_Square;
var gVbo_Position_Square;
var gVbo_Texture_Square;

checkImage[64][64][4];
var texImage;

var mvpUniform;

var perspectiveProjectionMatrix;

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
	var vertesShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"in vec2 vTexture;"+
	"uniform mat4 u_mvp_matrix;"+
	"out vec2 out_texture;"+
	"void main(void)"+
	"{"+
		"gl_Position = u_mvp_matrix * vPosition;"+
		"out_texture = vTexture;"+
	"}";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

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
	var fragmentShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec2 sampler2D u_texture_sampler;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
		"FragColor = texture(u_texture_sampler, out_texture);"+
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
	 gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");

	 gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, "vPosition");

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
	 gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

	 // *** vertices, colors, shader attribs, gVbo_Position_Square, gVao_Square initializations *** 
	 var squareVertices = new Float32Array([
	 									1.0, 1.0,
										0.0, 1.0,
										0.0, 0.0,
										1.0, 0.0
	 	]);


	gVao_Square = gl.createVertexArray();
	gl.bindVertexArray(gVao_Square);

	gVbo_Position_Square = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Position_Square);
	gl.bufferData(gl.ARRAY_BUFFER, 4 * 3 * sizeof(FLOAT), gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	


	gVbo_Texture_Square = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Texture_Square);
	gl.bufferData(gl.ARRAY_BUFFER, squareVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0,
		2, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
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
	straight[12] = [
			 0.0,  1.0, 0.0,
			-2.0,  1.0, 0.0,
			-2.0, -1.0, 0.0,
			 0.0, -1.0, 0.0,
	];

	angular[12] = [
			2.41421,	 1,		-1.41421,
			1,			 1,		 0,
			1,			-1,		 0,
			2.41421,	-1,		-1.41421
	]

	gl.clear(gl.COLOR_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrx = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);

	mat4.multiply(modelViewProjectionMatrx, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrx);
	

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, cube_texture);
	gl.uniform1i(textureSamplerUniform, 0);

	gl.bindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
	gl.bufferData(GL_ARRAY_BUFFER, 4 * 3, straight, GL_DYNAMIC_DRAW);
	gl.vertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(GL_ARRAY_BUFFER, 0);


	gl.bindVertexArray(gVao_Square);
	gl.drawArrays(gl.TRIANGLES, 0, 4);
	gl.bindVertexArray(null);

	gl.bindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
	gl.bufferData(GL_ARRAY_BUFFER, 4 * 3, angular, GL_DYNAMIC_DRAW);
	gl.vertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0, 3, gl.FLOAT, 0, 0);
	gl.enableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(GL_ARRAY_BUFFER, 0);

	gl.bindVertexArray(gVao_Square);
	gl.drawArrays(GL_TRIANGLE_FAN, 0, 4);
	gl.bindTexture(GL_TEXTURE_2D, 0);

	gl.useProgram(null);

	requestAnimationFrame(draw, canvas);

}

function uninitialize()
{
	// code
	if(gVao_Square)
	{
		gl.deleteVertexArray(gVao_Square);
		gVao_Square = null;
	}

	if(gVbo_Position_Square)
	{
		gl.deleteVertexArray(gVbo_Position_Square);
		gVbo_Position_Square = null;
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