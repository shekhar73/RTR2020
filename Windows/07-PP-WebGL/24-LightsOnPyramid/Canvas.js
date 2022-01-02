var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

var angle_pyramid = 0.0;

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

var bLight;

var vao_pyramid;
var vbo_position_pyramid;
var vbo_Normal_pyramid;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;
var LKeyPressedUniform;
var LaUniform_R;
var LdUniform_R;
var LsUniform_R;
var LaUniform_B;
var LdUniform_B;
var LsUniform_B;
var KaUniform;
var KdUniform;
var KsUniform;
var KShineUniform;
var lightPositionUniform_R;
var lightPositionUniform_B;

var lightPosition_R = [-2.0,0.0,0.0,1.0];
var lightAmbient_R 	= [0.1,0.1,0.1];
var lightDiffuse_R	= [1.0,0.0,0.0];
var lightSpecular_R	= [1.0,0.0,0.0];

var lightPosition_B = [2.0,0.0,0.0,1.0];
var lightAmbient_B 	= [0.1,0.1,0.1];
var lightDiffuse_B	= [0.0,0.0,1.0];
var lightSpecular_B	= [0.0,0.0,1.0];

var materialAmbient = [0.0,0.0,0.0];
var materialDiffuse	= [1.0,1.0,1.0];
var materialSpecular = [1.0,1.0,1.0];
var materialShininess = 50.0;

var perspectiveProjectionMatrix;


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
		console.log("Obataining Canvas Failed/n");
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


	// vertex shader

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode = 
	"#version 300 es"+
	"\n"+
    "in vec4 vPosition;" +
    "in vec3 vNormal;" +
    "uniform mat4 u_model_matrix;" +
    "uniform mat4 u_view_matrix;" +
    "uniform mat4 u_perspective_projection_matrix;" +
    "uniform vec3 u_la_red;" +
    "uniform vec3 u_ld_red;" +
    "uniform vec3 u_ls_red;" +
    "uniform vec4 u_light_position_red;" +
    "uniform vec3 u_la_blue;" +
    "uniform vec3 u_ld_blue;" +
    "uniform vec3 u_ls_blue;" +
    "uniform vec4 u_light_position_blue;" +
    "uniform vec3 u_ka;" +
    "uniform vec3 u_kd;" +
    "uniform vec3 u_ks;" +
    "uniform int u_LKeyPressed;" +  
    "uniform float u_shininess;" +
    "out vec3 phong_ads_light;" +
    "void main(void)" +
    "{" +   
    "if(u_LKeyPressed == 1)" +
    "{" +
    "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" +
    "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
    "vec3 view_vector = normalize(-eye_coordinates.xyz);" +

    "vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));" +
    "vec3 reflection_vector_red = reflect(-light_direction_red,transformed_normal);" +
    "vec3 ambient_red = u_la_red * u_ka;" +
    "vec3 diffuse_red = u_ld_red * u_kd * max(dot(light_direction_red,transformed_normal),0.0f);" +
    "vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,view_vector),0.0f),u_shininess);" +

    "vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" +
    "vec3 reflection_vector_blue = reflect(-light_direction_blue,transformed_normal);" +
    "vec3 ambient_blue = u_la_blue * u_ka;" +
    "vec3 diffuse_blue = u_ld_blue * u_kd * max(dot(light_direction_blue,transformed_normal),0.0f);" +
    "vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,view_vector),0.0f),u_shininess);" +

    "phong_ads_light = ambient_red + ambient_blue + diffuse_red + diffuse_blue + specular_red + specular_blue;" +
    "}" +
    "else" +
    "{" +
    "phong_ads_light = vec3(1.0f,1.0f,1.0f);" +
    "}" +
    "gl_Position = u_perspective_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
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

	// fragment shader
	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
    "in vec3 phong_ads_light;" +
    "out vec4 FragColor;" +
    "void main(void)" +
    "{" +
    "FragColor = vec4(phong_ads_light,1.0f);" +
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

	// shader program
	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject,vertexShaderObject);
	gl.attachShader(shaderProgramObject,fragmentShaderObject);

	//pre-link binding of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

	// linking
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

	// get MVP uniform location

	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_perspective_projection_matrix");	
	LaUniform_R = gl.getUniformLocation(shaderProgramObject, "u_la_red");
	LdUniform_R = gl.getUniformLocation(shaderProgramObject, "u_ld_red");
	LsUniform_R = gl.getUniformLocation(shaderProgramObject, "u_ls_red");
	LaUniform_B = gl.getUniformLocation(shaderProgramObject, "u_la_blue");
	LdUniform_B = gl.getUniformLocation(shaderProgramObject, "u_ld_blue");
	LsUniform_B = gl.getUniformLocation(shaderProgramObject, "u_ls_blue");
	KaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	KdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	KsUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	KShineUniform = gl.getUniformLocation(shaderProgramObject, "u_shininess");
	lightPositionUniform_R = gl.getUniformLocation(shaderProgramObject, "u_light_position_red");
	lightPositionUniform_B = gl.getUniformLocation(shaderProgramObject, "u_light_position_blue");
	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_LKeyPressed");

	// *** vertices, colors, shader attribs, vbo, vao initializations *** 
	var pyramidVertices = new Float32Array([
												0.0,	0.5,	0.0,
											   -0.5,	-0.5,	0.5,
								                0.5,	-0.5,	0.5,

												0.0,	0.5,	0.0,
												0.5,	-0.5,	0.5,
												0.5,	-0.5,	-0.5,

												0.0,	0.5,	0.0,
												0.5,	-0.5,	-0.5,
												-0.5,	-0.5,	-0.5,

											    0.0,	0.5,	0.0,
												-0.5,	-0.5,	-0.5,
												-0.5,	-0.5,	0.5
											]);

	var pyramidNormal = new Float32Array([
												0.0, 0.447214, 0.894427,
												0.0, 0.447214, 0.894427,
												0.0, 0.447214, 0.894427,
												0.0, 0.447214, 0.894427,
    
												0.894427, 0.447214, 0.0,
												0.894427, 0.447214, 0.0,
												0.894427, 0.447214, 0.0,
												0.894427, 0.447214, 0.0,
    
												0.0, 0.447214, -0.894427,
												0.0, 0.447214, -0.894427,
												0.0, 0.447214, -0.894427,
												0.0, 0.447214, -0.894427,
    
												-0.894427, 0.447214, 0.0,
												-0.894427, 0.447214, 0.0,
												-0.894427, 0.447214, 0.0,
												-0.894427, 0.447214, 0.0
										]);

	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);
    	vbo_position_pyramid = gl.createBuffer();
    	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position_pyramid);
    	gl.bufferData(gl.ARRAY_BUFFER,pyramidVertices,gl.STATIC_DRAW);
    	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,3,gl.FLOAT,false,0,0);
    	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
    	gl.bindBuffer(gl.ARRAY_BUFFER,null);

    	vbo_Normal_pyramid = gl.createBuffer();
    	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_Normal_pyramid);
    	gl.bufferData(gl.ARRAY_BUFFER,pyramidNormal,gl.STATIC_DRAW);
    	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_NORMAL,3,gl.FLOAT,false,0,0);
    	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);


	gl.bindVertexArray(null);

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

	gl.viewport(0,0,canvas.width,canvas.height);
	
	mat4.perspective(perspectiveProjectionMatrix,45.0,parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	if(bLight == true)
	{
		gl.uniform1i(LKeyPressedUniform, 1);
		gl.uniform1f(KShineUniform, materialShininess);

		gl.uniform4fv(lightPositionUniform_R, lightPosition_R);
		gl.uniform3fv(LaUniform_R, lightAmbient_R);
		gl.uniform3fv(LdUniform_R, lightDiffuse_R);
		gl.uniform3fv(LsUniform_R, lightSpecular_R);

		gl.uniform4fv(lightPositionUniform_B, lightPosition_B);
		gl.uniform3fv(LaUniform_B, lightAmbient_B);
		gl.uniform3fv(LdUniform_B, lightDiffuse_B);
		gl.uniform3fv(LsUniform_B, lightSpecular_B);

		gl.uniform3fv(KaUniform, materialAmbient);
		gl.uniform3fv(KdUniform, materialDiffuse);
		gl.uniform3fv(KsUniform, materialSpecular);
	}
	else
	{
		gl.uniform1i(LKeyPressedUniform,0);
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	projectionMatrix = perspectiveProjectionMatrix;

	mat4.translate(modelMatrix,modelMatrix,[0.0,0.0,-2.0]);
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(angle_pyramid));


	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	gl.bindVertexArray(vao_pyramid);

	gl.drawArrays(gl.TRIANGLES,0,12);

	gl.bindVertexArray(null);
	////////////////////////////////////////////////////////////////////////////////////////////////

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
	// code
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}

	if(vbo_position_pyramid)
	{
		gl.deleteVertexArray(vbo_position_pyramid);
		vbo_position_pyramid = null;
	}

	if(vbo_Normal_pyramid)
	{
		gl.deleteBuffer(vbo_Normal_pyramid);
		vbo_Normal_pyramid = null;
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