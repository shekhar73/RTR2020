// global variables

var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

const WebGLMacros = 
{
	SSK_ATTRIBUTE_POSITION:0,
    SSK_ATTRIBUTE_COLOR:1,
    SSK_ATTRIBUTE_NORMAL:2,
    SSK_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var lightPosition	 = [100.0, 100.0, 100.0, 1.0];
var lightAmbient	 = [0.1, 0.1, 0.1];
var lightDiffuse	 = [0.5, 0.2, 0.7];
var lightSpecular	 = [0.7, 0.7, 0.7];

var materialAmbient	 = [0.0, 0.0, 0.0];
var materialDiffuse	 = [1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0];

var materialShininess = 128.0;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var lightPositionUniform;
var LaUniform;
var LdUniform;
var LsUniform;

var KaUniform;
var KdUniform;
var KsUniform;
var KshineUniform;

var LKeyPressedUniform;
var bLight = false;

var sphere = null;

var perspectiveProjectionMatrix;

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
	canvas = document.getElementById("ssk_my_canvas");

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
			if(bLight == false)
				bLight = true;
			else
				bLight = false;
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


	// vertex shader

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"in vec3 vNormal;"+
	"uniform mat4 u_model_matrix;"+
	"uniform mat4 u_view_matrix;"+
	"uniform mat4 u_perspective_projection_matrix;"+
	"uniform vec4 u_light_position;"+
	"out vec3 transformed_normal;"+
	"out vec3 light_direction;"+
	"out vec3 view_vector;"+
	"void main(void)"+
	"{"+
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
		"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
		"light_direction = vec3(u_light_position - eye_coordinates);"+
		"view_vector = -eye_coordinates.xyz;"+
		"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
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

	// fragment shader
	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec3 transformed_normal;"+
	"in vec3 light_direction;"+
	"in vec3 view_vector;"+
	"uniform vec3 u_la;"+
	"uniform vec3 u_ld;"+
	"uniform vec3 u_ls;"+
	"uniform vec3 u_ka;"+
	"uniform vec3 u_kd;"+
	"uniform vec3 u_ks;"+
	"uniform highp float u_shininess;"+
	"uniform int u_LKeyPressed;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
		"vec3 phong_ads_light;"+
		"if(u_LKeyPressed == 1)"+
		"{"+
			"vec3 normalized_transformed_normal = normalize(transformed_normal);"+
			"vec3 normalized_light_direction = normalize(light_direction);"+
			"vec3 normalized_view_vector = normalize(view_vector);"+
			"vec3 ambient = u_la * u_ka;"+
			"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"+
			"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
			"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_shininess);"+
			"phong_ads_light = ambient + diffuse + specular;"+
		"}"+
		"else"+
		"{"+
			"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
		"}"+
		"FragColor = vec4(phong_ads_light, 1.0f);"+
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

	// linking
	gl.linkProgram(shaderProgramObject);

	if(gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false)
	{
	 	var error = gl.getProgramInfoLog(shaderProgramObject);
	 	if(error.length > 0)
	 	{
	 		alert(error);
	 		uninitialize();
	 	}
	}

	// get MVP uniform location
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_perspective_projection_matrix");
	LaUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
	LdUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	LsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
	KaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	KdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	KsUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	KshineUniform = gl.getUniformLocation(shaderProgramObject, "u_shininess");
	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_LKeyPressed");

	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	sphere = new Mesh();
	makeSphere(sphere, 2.0, 30, 30);

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

	if(bLight == true)
	{
		gl.uniform1i(LKeyPressedUniform, 1);
		gl.uniform1f(KshineUniform, materialShininess);
		gl.uniform4fv(lightPositionUniform, lightPosition);
		gl.uniform3fv(LaUniform, lightAmbient);
		gl.uniform3fv(LdUniform, lightDiffuse);
		gl.uniform3fv(LsUniform, lightSpecular);
		gl.uniform3fv(KaUniform, materialAmbient);
		gl.uniform3fv(KdUniform, materialDiffuse);
		gl.uniform3fv(KsUniform, materialSpecular);
	}
	else
	{
		gl.uniform1i(LKeyPressedUniform, 0);
	}

	var modelViewMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();
	var translateMatrix = mat4.create();

	modelMatrix = translateMatrix;
	projectionMatrix = perspectiveProjectionMatrix;
	
	mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0,-10.0]);

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();

	gl.useProgram(null);

	requestAnimationFrame(draw,canvas);
}

function uninitialize()
{
	if(sphere)
	{
		sphere.deallocate();
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