
var canvas = null;
var gl = null;	// webgl context
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

var angle_cube = 0.0;

const WebGLMacros = 
{
	SSK_ATTRIBUTE_POSITION:0,
    SSK_ATTRIBUTE_COLOR:1,
    SSK_ATTRIBUTE_NORMAL:2,
    SSK_ATTRIBUTE_TEXTURE0:3,

};

var lightPosition = [0.0, 0.0, 2.0, 1.0];

// Shaders
var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var cube_texture;

// Cube
var vao_cube;
var vbo_Position_cube;
var vbo_normal_cube;

var bAnimate = false;
var bLight = false;

var modelViewMatrixUniform;
var perspectiveProjectionUniform;
var lightPositionUniform;

var LdUniform;
var KdUniform;
var LKeyPressedUniform;

var perspeciveProjectionMatrix;

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

var canvas = document.getElementById('ssk_my_canvas');
var gl = canvas.getContext("webgl2");

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

		case 76:
		case 108:
			if(bLight == false)
				bLight = true;
			else
				bLight = false;
			break;

		case 65:
		case 97:
			if(bAnimate == false)
				bAnimate = true;
			else
				bAnimate = false;
			break;
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

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"in vec3 vNoraml;"+
	"uniform int u_LKeyPressed;"								+
	"uniform mat4 u_model_view_matrix;"+
	"uniform mat4 u_projection_matrix;"+
	"uniform vec3 u_ld;"+
	"uniform vec3 u_kd;"+
	"uniform vec4 u_light_position;"+
	"out vec3 diffuse_light;"+
	"void main(void)"+
	"{"+
		"if(u_LKeyPressed == 1)"								+
		"{"	+
			"vec4 eye_coordinates = u_model_view_matrix * vPosition;"+
			"mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));"+
			"vec3 tnorm = normalize(normal_matrix * vNoraml);"+
			"vec3 s = normalize(vec3(u_light_position - eye_coordinates));"+
			"diffuse_light = u_ld * u_kd * max(dot(s, tnorm), 0.0);"+
		"}"+
		"else"													+
		"{"														+
			"diffuse_light = vec3(1.0f);"					+
		"}"+
		
		"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;"+
	"}";

	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
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

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode = 
	"#version 300 es"											+
	"\n"														+
	"precision highp float;"									+
	"in vec3 diffuse_light;"									+
	"out vec4 FragColor;"										+
	"void main(void)"											+
	"{"															+
		"FragColor = vec4(diffuse_light, 1.0f);"				+
	"}";

	/*var fragmentShaderSourceCode = 
	"#version 300 es"											+
	"\n"														+
	"precision highp float;"									+
	"uniform int u_LKeyPressed;"								+
	"vec4 color;"												+
	"in vec3 diffuse_light;"									+
	"out vec4 FragColor;"										+
	"void main(void)"											+
	"{"															+
		"if(u_LKeyPressed == 1)"								+
		"{"														+
			"color = vec4(diffuse_light, 1.0f);"				+
		"}"														+
		"else"													+
		"{"														+
			"color = vec4(1.0, 1.0, 1.0, 1.0);"					+
		"}"														+
		"FragColor = color;"									+
	"}";*/

	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
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

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_POSITION,"vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_NORMAL,"vNoraml");

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

	modelViewMatrixUniform = gl.getUniformLocation(shaderProgramObject,"u_model_view_matrix");
	perspectiveProjectionUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_LKeyPressed");
	LdUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	KdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");

    var cubeVertices = new Float32Array([
							0.5,0.5,0.5,
                -0.5,0.5,0.5,
                -0.5,-0.5,0.5,
                0.5,-0.5,0.5,

                0.5,0.5,-0.5,
                0.5,0.5,0.5,
                0.5,-0.5,0.5,
                0.5,-0.5,-0.5,

                -0.5,0.5,-0.5,
                0.5,0.5,-0.5,
                0.5,-0.5,-0.5,
                -0.5,-0.5,-0.5,

                -0.5,0.5,0.5,
                -0.5,0.5,-0.5,
                -0.5,-0.5,-0.5,
                -0.5,-0.5,0.5,

                0.5,0.5,-0.5,
                -0.5,0.5,-0.5,
                -0.5,0.5,0.5,
                0.5,0.5,0.5,

                0.5,-0.5,-0.5,
                -0.5,-0.5,-0.5,
                -0.5,-0.5,0.5,
                0.5,-0.5,0.5
    ]);

	var cubeNormal = new Float32Array
	([
        0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,

		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,

		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0
	]);

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
		vbo_Position_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_Position_cube);
		gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER,null);

		vbo_normal_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER,vbo_normal_cube);
		gl.bufferData(gl.ARRAY_BUFFER, cubeNormal, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_NORMAL);
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

	if(bLight == true)
	{
		gl.uniform1i(LKeyPressedUniform, 1);
		gl.uniform3f(LdUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(KdUniform, 0.5, 0.5, 0.5);
		gl.uniform4fv(lightPositionUniform, lightPosition);
	}
	else
	{
		gl.uniform1i(LKeyPressedUniform, 0);
	}
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle_cube));
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_cube));
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(angle_cube));

	gl.uniformMatrix4fv(modelViewMatrixUniform, false, modelViewMatrix);
	gl.uniformMatrix4fv(perspectiveProjectionUniform, false, perspectiveProjectionMatrix);

	gl.bindVertexArray(vao_cube);

	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	

	gl.bindVertexArray(null);
	gl.useProgram(null);

	if(bAnimate == true)
	{
		angle_cube = angle_cube + 1.0;
	}
	if(angle_cube >= 360.0)
	{
		angle_cube = 0.0;
	}
	requestAnimationFrame(draw, canvas);
}

function degToRad(degree)
{
	return(degree * Math.PI / 180.0);
}

function uninitialize()
{
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

	if(vbo_normal_cube)
	{
		gl.deleteBuffer(vbo_normal_cube);
		vbo_normal_cube = null;
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