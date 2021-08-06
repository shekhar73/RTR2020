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

var bToggleLights;

var vertexShaderObject_VS;
var fragmentShaderObject_VS;
var shaderProgramObject_VS;

var vertexShaderObject_FS;
var fragmentShaderObject_FS;
var shaderProgramObject_FS;

var lightPosition	 = [100.0, 100.0, 100.0, 1.0];
var lightAmbient	 = [0.1, 0.1, 0.1];
var lightDiffuse	 = [0.5, 0.2, 0.7];
var lightSpecular	 = [0.7, 0.7, 0.7];

var materialAmbient	 = [0.0, 0.0, 0.0];
var materialDiffuse	 = [1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0];

var materialShininess = 128.0;

var modelMatrixUniform_VS;
var viewMatrixUniform_VS;
var projectionMatrixUniform_VS;

var lightPositionUniform_VS;
var LaUniform_VS;
var LdUniform_VS;
var LsUniform_VS;

var KaUniform_VS;
var KdUniform_VS;
var KsUniform_VS;
var KshineUniform_VS;

var LKeyPressedUniform_VS;

var modelMatrixUniform_FS;
var viewMatrixUniform_FS;
var projectionMatrixUniform_FS;

var lightPositionUniform_FS;
var LaUniform_FS;
var LdUniform_FS;
var LsUniform_FS;

var KaUniform_FS;
var KdUniform_FS;
var KsUniform_FS;
var KshineUniform_FS;

var LKeyPressedUniform_FS;

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
		// Q
		case 81:
		case 113:
			uninitialize();
			window.close();
			break;

		case 27: // Escape
			toggleFullscreen();
			break;

		// L
		case 76:
		case 108:
			if(bLight == false)
				bLight = true;
			else
				bLight = false;
			break;

		// F
		case 70:
		case 102:
			bToggleLights = false;
			break;

		// V
		case 118:
		case 86:
			bToggleLights = true;
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

	vertexShaderObject_VS = gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode_VS = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"in vec3 vNormal;"+
	"uniform mat4 u_model_matrix;"+
	"uniform mat4 u_view_matrix;"+
	"uniform mat4 u_perspective_projection_matrix;"+
	"uniform vec3 u_la;"+
	"uniform vec3 u_ld;"+
	"uniform vec3 u_ls;"+
	"uniform vec3 u_ka;"+
	"uniform vec3 u_kd;"+
	"uniform vec3 u_ks;"+
	"uniform vec4 u_light_position;"+
	"uniform int u_LKeyPressed;"+
	"uniform float u_shininess;"+
	"out vec3 phong_ads_light;"+
	"void main(void)"+
	"{"+
		"if(u_LKeyPressed == 1)"+
		"{"+
			"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
			"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
			"vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));"+
			"vec3 reflection_vector = reflect(-light_direction, transformed_normal);"+
			"vec3 view_vector = normalize(-eye_coordinates.xyz);"+
			"vec3 ambient = u_la * u_ka;"+
			"vec3 diffuse = u_ld * u_kd * max(dot(light_direction, transformed_normal), 0.0f);"+
			"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, view_vector), 0.0f), u_shininess);"+
			"phong_ads_light = ambient + diffuse + specular;"+
		"}"+
		"else"+
		"{"+
			"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
		"}"+
	"gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
	"}";

	gl.shaderSource(vertexShaderObject_VS, vertexShaderSourceCode_VS);
	gl.compileShader(vertexShaderObject_VS);

	if(gl.getShaderParameter(vertexShaderObject_VS, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject_VS);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// fragment shader
	fragmentShaderObject_VS = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode_VS = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec3 phong_ads_light;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
		"FragColor = vec4(phong_ads_light, 1.0f);"+
	"}";

	gl.shaderSource(fragmentShaderObject_VS, fragmentShaderSourceCode_VS);
	gl.compileShader(fragmentShaderObject_VS);

	if(gl.getShaderParameter(fragmentShaderObject_VS, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject_VS);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// shader program
	shaderProgramObject_VS = gl.createProgram();

	gl.attachShader(shaderProgramObject_VS, vertexShaderObject_VS);
	gl.attachShader(shaderProgramObject_VS, fragmentShaderObject_VS);

	//pre-link binding of shader program object with vertex shader attributes

	gl.bindAttribLocation(shaderProgramObject_VS, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_VS, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

	// linking
	gl.linkProgram(shaderProgramObject_VS);

	if(gl.getProgramParameter(shaderProgramObject_VS, gl.LINK_STATUS) == false)
	{
	 	var error = gl.getProgramInfoLog(shaderProgramObject_VS);
	 	if(error.length > 0)
	 	{
	 		alert(error);
	 		uninitialize();
	 	}
	}

	// get MVP uniform location
	modelMatrixUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_model_matrix");
	viewMatrixUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_view_matrix");
	projectionMatrixUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_perspective_projection_matrix");
	LaUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_la");
	LdUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_ld");
	LsUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_ls");
	lightPositionUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_light_position");
	KaUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_ka");
	KdUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_kd");
	KsUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_ks");
	KshineUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_shininess");
	LKeyPressedUniform_VS = gl.getUniformLocation(shaderProgramObject_VS, "u_LKeyPressed");

//------------------------------------------------------------------------------------------------------------------------------


// vertex shader

	vertexShaderObject_FS = gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode_FS = 
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

	gl.shaderSource(vertexShaderObject_FS, vertexShaderSourceCode_FS);
	gl.compileShader(vertexShaderObject_FS);

	if(gl.getShaderParameter(vertexShaderObject_FS, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject_FS);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// fragment shader
	fragmentShaderObject_FS = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode_FS = 
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


	gl.shaderSource(fragmentShaderObject_FS, fragmentShaderSourceCode_FS);
	gl.compileShader(fragmentShaderObject_FS);

	if(gl.getShaderParameter(fragmentShaderObject_FS, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject_FS);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// shader program
	shaderProgramObject_FS = gl.createProgram();

	gl.attachShader(shaderProgramObject_FS, vertexShaderObject_FS);
	gl.attachShader(shaderProgramObject_FS, fragmentShaderObject_FS);

	//pre-link binding of shader program object with vertex shader attributes

	gl.bindAttribLocation(shaderProgramObject_FS, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_FS, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

	// linking
	gl.linkProgram(shaderProgramObject_FS);

	if(gl.getProgramParameter(shaderProgramObject_FS, gl.LINK_STATUS) == false)
	{
	 	var error = gl.getProgramInfoLog(shaderProgramObject_FS);
	 	if(error.length > 0)
	 	{
	 		alert(error);
	 		uninitialize();
	 	}
	}

	// get MVP uniform location
	modelMatrixUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_model_matrix");
	viewMatrixUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_view_matrix");
	projectionMatrixUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_perspective_projection_matrix");
	LaUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_la");
	LdUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_ld");
	LsUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_ls");
	lightPositionUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_light_position");
	KaUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_ka");
	KdUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_kd");
	KsUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_ks");
	KshineUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_shininess");
	LKeyPressedUniform_FS = gl.getUniformLocation(shaderProgramObject_FS, "u_LKeyPressed");

//--------------------------------------------------------------------------------------------------------------------------	
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

	if(bToggleLights == true)
	{
		gl.useProgram(shaderProgramObject_VS);

		if(bLight == true)
		{
			gl.uniform1i(LKeyPressedUniform_VS, 1);
			gl.uniform1f(KshineUniform_VS, materialShininess);
			gl.uniform4fv(lightPositionUniform_VS, lightPosition);
			gl.uniform3fv(LaUniform_VS, lightAmbient);
			gl.uniform3fv(LdUniform_VS, lightDiffuse);
			gl.uniform3fv(LsUniform_VS, lightSpecular);
			gl.uniform3fv(KaUniform_VS, materialAmbient);
			gl.uniform3fv(KdUniform_VS, materialDiffuse);
			gl.uniform3fv(KsUniform_VS, materialSpecular);
		}
		else
		{
			gl.uniform1i(LKeyPressedUniform_VS, 0);
		}

		var modelViewMatrix = mat4.create();
		var viewMatrix = mat4.create();
		var projectionMatrix = mat4.create();
		var translateMatrix = mat4.create();

		modelMatrix = translateMatrix;
		projectionMatrix = perspectiveProjectionMatrix;
		
		mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0,-10.0]);

		gl.uniformMatrix4fv(modelMatrixUniform_VS, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniform_VS, false, viewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniform_VS, false, projectionMatrix);
	}
	else
	{
		gl.useProgram(shaderProgramObject_FS);

		if(bLight == true)
		{
			gl.uniform1i(LKeyPressedUniform_FS, 1);
			gl.uniform1f(KshineUniform_FS, materialShininess);
			gl.uniform4fv(lightPositionUniform_FS, lightPosition);
			gl.uniform3fv(LaUniform_FS, lightAmbient);
			gl.uniform3fv(LdUniform_FS, lightDiffuse);
			gl.uniform3fv(LsUniform_FS, lightSpecular);
			gl.uniform3fv(KaUniform_FS, materialAmbient);
			gl.uniform3fv(KdUniform_FS, materialDiffuse);
			gl.uniform3fv(KsUniform_FS, materialSpecular);
		}
		else
		{
			gl.uniform1i(LKeyPressedUniform_FS, 0);
		}

		var modelViewMatrix = mat4.create();
		var viewMatrix = mat4.create();
		var projectionMatrix = mat4.create();
		var translateMatrix = mat4.create();

		modelMatrix = translateMatrix;
		projectionMatrix = perspectiveProjectionMatrix;
		
		mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0,-10.0]);

		gl.uniformMatrix4fv(modelMatrixUniform_FS, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniform_FS, false, viewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniform_FS, false, projectionMatrix);
	}
	
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
	
	if(shaderProgramObject_VS)
	{
		if(fragmentShaderObject_VS)
		{
			gl.detachShader(shaderProgramObject_VS, fragmentShaderObject_VS);
			gl.deleteShader(fragmentShaderObject_VS);
			fragmentShaderObject_VS = null;
		}

		if(vertexShaderObject_VS)
		{
			gl.detachShader(shaderProgramObject_VS, vertexShaderObject_VS);
			gl.deleteShader(vertexShaderObject_VS);
			vertexShaderObject_VS = null;
		}

		gl.deleteProgram(shaderProgramObject_VS);
		shaderProgramObject_VS = null;
	}

	if(shaderProgramObject_FS)
	{
		if(fragmentShaderObject_FS)
		{
			gl.detachShader(shaderProgramObject_FS, fragmentShaderObject_FS);
			gl.deleteShader(fragmentShaderObject_FS);
			fragmentShaderObject_FS = null;
		}

		if(vertexShaderObject_FS)
		{
			gl.detachShader(shaderProgramObject_FS, vertexShaderObject_FS);
			gl.deleteShader(vertexShaderObject_FS);
			vertexShaderObject_FS = null;
		}

		gl.deleteProgram(shaderProgramObject_FS);
		shaderProgramObject_FS = null;
	}
}
