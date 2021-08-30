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

var factor = 10.0;
var angleX = 0.0;
var angleY = 0.0;
var angleZ = 0.0;

var lightPosition_Red 	= [0.0, 0.0, 0.0, 1.0];
var lightAmbient_Red 	= [0.0, 0.0, 0.0];
var lightDiffuse_Red 	= [1.0, 0.0, 0.0];
var lightSpecular_Red 	= [1.0, 0.0, 0.0];

var lightPosition_Blue 	= [0.0, 0.0, 0.0, 1.0];
var lightAmbient_Blue 	= [0.0, 0.0, 0.0];
var lightDiffuse_Blue 	= [0.0, 0.0, 1.0];
var lightSpecular_Blue 	= [0.0, 0.0, 1.0];

var lightPosition_Green = [0.0, 0.0, 0.0, 1.0];
var lightAmbient_Green 	= [0.0, 0.0, 0.0];
var lightDiffuse_Green 	= [0.0, 1.0, 0.0];
var lightSpecular_Green = [0.0, 1.0, 0.0];

var MaterialAmbient 	= [0.0, 0.0, 0.0];
var MaterialDiffuse 	= [1.0, 1.0, 1.0];
var MaterialSpecular 	= [1.0, 1.0, 1.0];
var MaterialShininess 	= 128.0;


var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var lightPositionUniform_Red;
var LaUniform_Red;
var LdUniform_Red;
var LsUniform_Red;

var lightPositionUniform_Blue;
var LaUniform_Blue;
var LdUniform_Blue;
var LsUniform_Blue;

var lightPositionUniform_Green;
var LaUniform_Green;
var LdUniform_Green;
var LsUniform_Green;

var KaUniform;
var KdUniform;
var KsUniform;
var KshineUniform;
var LKeyPressedUniform;

var sphere = null;
var bLight = false;

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

function toggleFullScreen()
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

		case 70:
		case 102:
			toggleFullScreen(); 
			break;

		case 76:
		case 108:
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
	"uniform vec3 u_la_red;"+
	"uniform vec3 u_ld_red;"+
	"uniform vec3 u_ls_red;"+
	"uniform vec4 u_light_position_red;"+
	"uniform vec3 u_la_blue;"+
	"uniform vec3 u_ld_blue;"+
	"uniform vec3 u_ls_blue;"+
	"uniform vec4 u_light_position_blue;"+
	"uniform vec3 u_la_green;"+
	"uniform vec3 u_ld_green;"+
	"uniform vec3 u_ls_green;"+
	"uniform vec4 u_light_position_green;"+
	"uniform vec3 u_ka;"+
	"uniform vec3 u_kd;"+
	"uniform vec3 u_ks;"+
	"uniform int u_LKeyPressed;"+
	"uniform float u_shininess;"+
	"out vec3 phong_ads_light;"+
	"void main(void)"+
	"{"+
		"if(u_LKeyPressed == 1)"+
		"{"+
			"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
			"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
			"vec3 view_vector = normalize(-eye_coordinates.xyz);"+

			"vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));"+
			"vec3 reflection_vector_red = reflect(-light_direction_red, transformed_normal);"+
			"vec3 ambient_red = u_la_red * u_ka;"+
			"vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red, transformed_normal), 0.0f);"+
			"vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red, view_vector), 0.0f), u_shininess);"+

			"vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));"+
			"vec3 reflection_vector_blue = reflect(-light_direction_blue, transformed_normal);"+
			"vec3 ambient_blue = u_la_blue * u_ka;"+
			"vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue, transformed_normal), 0.0f);"+
			"vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue, view_vector), 0.0f), u_shininess);"+

			"vec3 light_direction_green = normalize(vec3(u_light_position_green - eye_coordinates));"+
			"vec3 reflection_vector_green = reflect(-light_direction_green, transformed_normal);"+
			"vec3 ambient_green = u_la_green * u_ka;"+
			"vec3 diffuse_green = u_ld_green * u_kd * max(dot(light_direction_green, transformed_normal), 0.0f);"+
			"vec3 specular_green = u_ls_green * u_ks * pow(max(dot(reflection_vector_green, view_vector), 0.0f), u_shininess);"+

			"phong_ads_light = ambient_red + ambient_blue + ambient_green + diffuse_red + diffuse_blue + diffuse_green + specular_red + specular_blue + specular_green;"+		
		"}"+
		"else"+
		"{"+
			"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
		"}"+
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
	"in vec3 phong_ads_light;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
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

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

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


	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject,"u_perspective_projection_matrix");
    LaUniform_Red = gl.getUniformLocation(shaderProgramObject,"u_la_red");
    LdUniform_Red = gl.getUniformLocation(shaderProgramObject,"u_ld_red");
   	LsUniform_Red = gl.getUniformLocation(shaderProgramObject,"u_ls_red");
    lightPositionUniform_Red = gl.getUniformLocation(shaderProgramObject,"u_light_position_red");
    LaUniform_Blue = gl.getUniformLocation(shaderProgramObject,"u_la_blue");
    LdUniform_Blue = gl.getUniformLocation(shaderProgramObject,"u_ld_blue");
   	LsUniform_Blue = gl.getUniformLocation(shaderProgramObject,"u_ls_blue");
    lightPositionUniform_Blue = gl.getUniformLocation(shaderProgramObject,"u_light_position_blue");
    LaUniform_Green = gl.getUniformLocation(shaderProgramObject,"u_la_green");
    LdUniform_Green = gl.getUniformLocation(shaderProgramObject,"u_ld_green");
   	LsUniform_Green = gl.getUniformLocation(shaderProgramObject,"u_ls_green");
    lightPositionUniform_Green = gl.getUniformLocation(shaderProgramObject,"u_light_position_green");
    KaUniform = gl.getUniformLocation(shaderProgramObject,"u_ka");
    KdUniform = gl.getUniformLocation(shaderProgramObject,"u_kd");
    KsUniform = gl.getUniformLocation(shaderProgramObject,"u_ks");
    KshineUniform = gl.getUniformLocation(shaderProgramObject,"u_shininess");
    LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject,"u_LKeyPressed");


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
		lightPosition_Red[0] = 0.0;
		lightPosition_Red[1] = factor * Math.sin(angleX);
		lightPosition_Red[2] = factor * Math.cos(angleX);
		lightPosition_Red[3] = 1.0;

        lightPosition_Green[0] = factor * Math.sin(angleY);
		lightPosition_Green[1] = 0.0;
		lightPosition_Green[2] = factor * Math.cos(angleY);
		lightPosition_Green[3] = 1.0;

		lightPosition_Blue[0] = factor * Math.sin(angleZ);
		lightPosition_Blue[1] = factor * Math.cos(angleZ);
		lightPosition_Blue[2] = 0.0;
		lightPosition_Blue[3] = 1.0;
		

		gl.uniform1i(LKeyPressedUniform, 1);
		gl.uniform1f(KshineUniform, MaterialShininess);

		gl.uniform4fv(lightPositionUniform_Red, lightPosition_Red);
		gl.uniform3fv(LaUniform_Red, lightAmbient_Red);
		gl.uniform3fv(LdUniform_Red, lightDiffuse_Red);
		gl.uniform3fv(LsUniform_Red, lightSpecular_Red);

		gl.uniform4fv(lightPositionUniform_Blue, lightPosition_Blue);
		gl.uniform3fv(LaUniform_Blue, lightAmbient_Blue);
		gl.uniform3fv(LdUniform_Blue, lightDiffuse_Blue);
		gl.uniform3fv(LsUniform_Blue, lightSpecular_Blue);

		gl.uniform4fv(lightPositionUniform_Green, lightPosition_Green);
		gl.uniform3fv(LaUniform_Green, lightAmbient_Green);
		gl.uniform3fv(LdUniform_Green, lightDiffuse_Green);
		gl.uniform3fv(LsUniform_Green, lightSpecular_Green);

		gl.uniform3fv(KaUniform, MaterialAmbient);
		gl.uniform3fv(KdUniform, MaterialDiffuse);
		gl.uniform3fv(KsUniform, MaterialSpecular);
	}
	else
	{
		gl.uniform1i(LKeyPressedUniform, 0);
	}

	var modelViewMatrix = mat4.create();
	var viewMatrix 	= mat4.create();
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

	angleX = angleX + 0.1;
	if(angleX >= 360.0)
	{
		angleX = 0.0;
	}

	angleY = angleY + 0.1;
	if(angleY >= 360.0)
	{
		angleY = 0.0;
	}

	angleZ = angleZ + 0.1;
	if(angleZ >= 360.0)
	{
		angleZ = 0.0;
	}

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
