"use strict";
var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

const WebGLMacros =
{
	SSK_ATTRIBUTE_VERTEX: 0,
	SSK_ATTRIBUTE_COLOR: 1,
	SSK_ATTRIBUTE_NORMAL: 2,
	SSK_ATTRIBUTE_TEXTURE: 3
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var sphere_vertices;
var sphere_normals;

var vao_sphere;
var vbo_vertices_sphere;
var vbo_normals_sphere;

var modelMatrixUniform;
var viewMatrixUniform;
var perspectiveProjectionUniform;
var lKeyPressedUniform;

var LightPositionUniform;
var LightSpecularUniform;
var LightDiffuseUniform;
var LightAmbientUniform;

var KaUniform;
var KdUniform;
var KsUniform;
var MaterialShineUniform;

var bAnimateSpheres = false;
var bLight = false;

var perspectiveProjectionMatrix;

var xRotation = 1;

var LightAmbient = new Float32Array([0.0, 0.0, 0.0]);
var LightDiffuse = new Float32Array([1.0, 1.0, 1.0]);
var LightSpecular = new Float32Array([1.0, 1.0, 1.0]);
var LightPosition = new Float32Array([0.0, 0.0, 0.0, 0.0]);

var lightRotateAngleX = 0.0;

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


function invokeVerticesOfSphere(radius, lats, longs) {
	var vertices = Array();

	const rad = Math.PI / 180.0;
	const latitudesValues = 180.0 / lats;
	const longitudesValues = 360.0 / longs;

	for (var i = 0.0; i < 360.0; i += longitudesValues) {
		var iNextValues = i + longitudesValues;
		for (var j = 0.0; j < 180.0; j += latitudesValues) {
			var jNextValues = j + latitudesValues;

			var points = Array(4);
			var x, y, z;

			x = radius * Math.cos(i * rad) * Math.sin(j * rad);
			y = radius * Math.cos(j * rad);
			z = radius * Math.sin(i * rad) * Math.sin(j * rad);
			points[0] = Array(x, y, z);

			x = radius * Math.cos(iNextValues * rad) * Math.sin(j * rad);
			y = radius * Math.cos(j * rad);
			z = radius * Math.sin(iNextValues * rad) * Math.sin(j * rad);
			points[1] = Array(x, y, z);

			x = radius * Math.cos(iNextValues * rad) * Math.sin(jNextValues * rad);
			y = radius * Math.cos(jNextValues * rad);
			z = radius * Math.sin(iNextValues * rad) * Math.sin(jNextValues * rad);
			points[2] = Array(x, y, z);

			x = radius * Math.cos(i * rad) * Math.sin(jNextValues * rad);
			y = radius * Math.cos(jNextValues * rad);
			z = radius * Math.sin(i * rad) * Math.sin(jNextValues * rad);
			points[3] = Array(x, y, z);

			var tri1 = points[0].concat(points[1], points[2]);
			var tri2 = points[2].concat(points[3], points[0]);

			vertices = vertices.concat(tri1, tri2);
		}
	}
	return Float32Array.from(vertices);
}


function normalsOfSphere(lats, longs) {
	var normals = Array();

	const rad = Math.PI / 180.0;
	const latitudesValues = 180.0 / lats;
	const longitudesValues = 360.0 / longs;

	for (var i = 0.0; i < 360.0; i += longitudesValues) 
	{
		var iNextValues = i + longitudesValues;
		for (var j = 0.0; j < 180.0; j += latitudesValues) 
		{
			var jNextValues = j + latitudesValues;

			var points = Array(4);
			var x, y, z;

			x = Math.cos(i * rad) * Math.sin(j * rad);
			y = Math.cos(j * rad);
			z = Math.sin(i * rad) * Math.sin(j * rad);
			points[0] = Array(x, y, z);

			x = Math.cos(iNextValues * rad) * Math.sin(j * rad);
			y = Math.cos(j * rad);
			z = Math.sin(iNextValues * rad) * Math.sin(j * rad);
			points[1] = Array(x, y, z);

			x = Math.cos(iNextValues * rad) * Math.sin(jNextValues * rad);
			y = Math.cos(jNextValues * rad);
			z = Math.sin(iNextValues * rad) * Math.sin(jNextValues * rad);
			points[2] = Array(x, y, z);

			x = Math.cos(i * rad) * Math.sin(jNextValues * rad);
			y = Math.cos(jNextValues * rad);
			z = Math.sin(i * rad) * Math.sin(jNextValues * rad);
			points[3] = Array(x, y, z);

			var tri1 = points[0].concat(points[1], points[2]);
			var tri2 = points[2].concat(points[3], points[0]);

			var tri1_norm = Array(3);
			var tri2_norm = Array(3);
			for (var n = 0; n < 3; n++) {
				tri1_norm[n] = (tri1[0 + n] + tri1[3 + n] + tri1[6 + n]) / 3;
				tri2_norm[n] = (tri2[0 + n] + tri2[3 + n] + tri2[6 + n]) / 3;
			}

			normals = normals.concat(tri1, tri2);
		}
	}
	return Float32Array.from(normals);
}

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

	resize();	// warmup repaint call

	draw();	// warmup repaint call
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

function keyDown(event)
{
	switch (event.keyCode)
	{
		case 76:
			if (bLight == true) 
			{
				bLight = false;
			}
			else 
			{
				bLight = true;
			}
			break;

		case 65:
			if (bAnimateSpheres == true) 
			{
				bAnimateSpheres = false;
			}
			else 
			{
				bAnimateSpheres = true;
			}
			break;

		case 70:
            toggleFullscreen();
            break;

		case 27:
			uninitialize();
			window.close();
			break;

		case 88:
			xRotation = 1;
			break;

		case 89:
			xRotation = 2;
			break;

		case 90:
			xRotation = 3;
			break;

		default:
			break;
	}
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
	var vertexShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_modelMatrix;" +
		"uniform mat4 u_viewMatrix;" +
		"uniform mat4 u_projectionMatrix;" +
		"uniform int u_lKeyPressed;" +
		"uniform vec4 light_position;" +
		"out vec3 transformed_Normal;" +
		"out vec3 light_direction;" +
		"out vec3 view_vector;" +
		"void main(void)" +
		"{" +
		"if(u_lKeyPressed == 1){" +
		"vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" +
		"transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" +
		"light_direction = vec3(light_position - eyeCoordinate);" +
		"view_vector = -eyeCoordinate.xyz;" +
		"}" +
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" +
		"}";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);

	if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
        }
	}

	// fragment shader
	var fragmentShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"vec3 phong_ads_light;" +
		"in vec3 transformed_Normal;" +
		"in vec3 light_direction;" +
		"in vec3 view_vector;" +
		"out vec4 FragColor;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ls;" +
		"uniform vec4 light_position;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_ks;" +
		"uniform float materialShineUniform;" +
		"uniform highp int u_lKeyPressed;" +
		"void main(void)" +
		"{" +
		"if(u_lKeyPressed == 1){" +
		"vec3 normalizedTransformNormal = normalize(transformed_Normal);" +
		"vec3 normalizedLightDirection = normalize(light_direction);" +
		"vec3 normalizedViewVector = normalize(view_vector);" +
		"vec3 ambient = u_la * u_ka;" +
		"vec3 diffuse = u_ld * u_kd * max(dot(normalizedLightDirection, normalizedTransformNormal) , 0.0f);" +
		"vec3 reflection_vector = reflect(-normalizedLightDirection, normalizedTransformNormal);" +
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalizedViewVector), 0.0f) ,  materialShineUniform);" +
		"phong_ads_light = ambient + diffuse + specular;" +
		"}" +
		"else{" +
		"phong_ads_light = vec3(1.0,1.0,1.0f);" +
		"}" +
		"FragColor = vec4(phong_ads_light, 1.0);" +
		"}";

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	
	if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");

	gl.linkProgram(shaderProgramObject);
	if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_viewMatrix");
	perspectiveProjectionUniform = gl.getUniformLocation(shaderProgramObject, "u_projectionMatrix");
	lKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_lKeyPressed");
	LightAmbientUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
	LightDiffuseUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	LightSpecularUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
	LightPositionUniform = gl.getUniformLocation(shaderProgramObject, "light_position");
	KdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	KaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	KsUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	MaterialShineUniform = gl.getUniformLocation(shaderProgramObject, "materialShineUniform");

	sphere_vertices = invokeVerticesOfSphere(0.5, 40, 40);
	sphere_normals = normalsOfSphere(40, 40);

	vao_sphere = gl.createVertexArray();
	gl.bindVertexArray(vao_sphere);

	vbo_vertices_sphere = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_vertices_sphere);
	gl.bufferData(gl.ARRAY_BUFFER, sphere_vertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);


	vbo_normals_sphere = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normals_sphere);
	gl.bufferData(gl.ARRAY_BUFFER, sphere_normals, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_NORMAL);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);


	gl.bindVertexArray(null);

	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);

	gl.clearColor(0.1, 0.1, 0.1, 1.0);

	perspectiveProjectionMatrix = mat4.create();
}

function resize()
{
	if (bFullscreen == true)
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
	var materialAmbient = new Float32Array(3);
	var materialDiffuse = new Float32Array(3);
	var materialSpecular = new Float32Array(3);
	var materialShine;


	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	gl.uniform3fv(LightAmbientUniform, LightAmbient);
	gl.uniform3fv(LightDiffuseUniform, LightDiffuse);
	gl.uniform3fv(LightSpecularUniform, LightSpecular);
	gl.uniform4fv(LightPositionUniform, LightPosition); // Here 4fv needed all else can be if needed 3fv

	var viewMatrix = mat4.create();
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(perspectiveProjectionUniform, false, perspectiveProjectionMatrix);

	if (bLight == true)
	{
		gl.uniform1i(lKeyPressedUniform, 1);
	}
	else 
	{
		gl.uniform1i(lKeyPressedUniform, 0);
	}

	var cWidth = canvas.width;
	var cHeight = canvas.height;

	// || 1 Sphere|| Emerald ----------------------------------------------------------------------
	// start using OpenGL program object
	gl.viewport(0, cHeight / 6 * 4, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0215; materialAmbient[1] = 0.1745; materialAmbient[2] = 0.0215;
	materialDiffuse[0] = 0.07568; materialDiffuse[1] = 0.61424; materialDiffuse[2] = 0.07568;
	materialSpecular[0] = 0.633; materialSpecular[1] = 0.727811; materialSpecular[2] = 0.633;
	materialShine = 0.6 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);


	// Jade || 2 Sphere||----------------------------------------------------------------------
	gl.viewport(cWidth / 6, cHeight / 6 * 4, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.135; materialAmbient[1] = 0.2225; materialAmbient[2] = 0.1575;
	materialDiffuse[0] = 0.54; materialDiffuse[1] = 0.89; materialDiffuse[2] = 0.63;
	materialSpecular[0] = 0.316228; materialSpecular[1] = 0.316228; materialSpecular[2] = 0.316228;
	materialShine = 0.1 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);

	// || obsidian 3 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 2, cHeight / 6 * 4, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.05375; materialAmbient[1] = 0.05; materialAmbient[2] = 0.06625;
	materialDiffuse[0] = 0.18275; materialDiffuse[1] = 0.17; materialDiffuse[2] = 0.22525;
	materialSpecular[0] = 0.332741; materialSpecular[1] = 0.328634; materialSpecular[2] = 0.346435;
	materialShine = 0.3 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || pearl 4 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 3, cHeight / 6 * 4, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.25; materialAmbient[1] = 0.20725; materialAmbient[2] = 0.20725;
	materialDiffuse[0] = 1.0; materialDiffuse[1] = 0.829; materialDiffuse[2] = 0.829;
	materialSpecular[0] = 0.296648; materialSpecular[1] = 0.296648; materialSpecular[2] = 0.296648;
	materialShine = 0.088 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || ruby 5 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 4, cHeight / 6 * 4, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.1745; materialAmbient[1] = 0.01175; materialAmbient[2] = 0.01175;
	materialDiffuse[0] = 0.61424; materialDiffuse[1] = 0.04136; materialDiffuse[2] = 0.04136;
	materialSpecular[0] = 0.727811; materialSpecular[1] = 0.626959; materialSpecular[2] = 0.626959;
	materialShine = 0.6 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || turquoise 6 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 5, cHeight / 6 * 4, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.1745; materialAmbient[1] = 0.01175; materialAmbient[2] = 0.01175;
	materialDiffuse[0] = 0.61424; materialDiffuse[1] = 0.04136; materialDiffuse[2] = 0.04136;
	materialSpecular[0] = 0.727811; materialSpecular[1] = 0.626959; materialSpecular[2] = 0.626959;
	materialShine = 0.6 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// ||brass 7 Sphere||----------------------------------------------------------------------

	gl.viewport(0, cHeight / 6 * 3, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.329412; materialAmbient[1] = 0.223529; materialAmbient[2] = 0.027451;
	materialDiffuse[0] = 0.780392; materialDiffuse[1] = 0.568627; materialDiffuse[2] = 0.113725;
	materialSpecular[0] = 0.992157; materialSpecular[1] = 0.941176; materialSpecular[2] = 0.807843;
	materialShine = 0.21794872 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// || bronze 8 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6, cHeight / 6 * 3, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.2125; materialAmbient[1] = 0.1275; materialAmbient[2] = 0.054;
	materialDiffuse[0] = 0.714; materialDiffuse[1] = 0.4284; materialDiffuse[2] = 0.18144;
	materialSpecular[0] = 0.393548; materialSpecular[1] = 0.271906; materialSpecular[2] = 0.166721;
	materialShine = 0.2 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// || chrome 9 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 2, cHeight / 6 * 3, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.255; materialAmbient[1] = 0.25; materialAmbient[2] = 0.25;
	materialDiffuse[0] = 0.4; materialDiffuse[1] = 0.4; materialDiffuse[2] = 0.4;
	materialSpecular[0] = 0.774597; materialSpecular[1] = 0.774597; materialSpecular[2] = 0.774597;
	materialShine = 0.6 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || copper 10 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 3, cHeight / 6 * 3, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.19125; materialAmbient[1] = 0.0735; materialAmbient[2] = 0.0225;
	materialDiffuse[0] = 0.7038; materialDiffuse[1] = 0.27048; materialDiffuse[2] = 0.0828;
	materialSpecular[0] = 0.256777; materialSpecular[1] = 0.137622; materialSpecular[2] = 0.086014;
	materialShine = 0.1 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || gold 11 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 4, cHeight / 6 * 3, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.24725; materialAmbient[1] = 0.1995; materialAmbient[2] = 0.0745;
	materialDiffuse[0] = 0.75164; materialDiffuse[1] = 0.60648; materialDiffuse[2] = 0.22648;
	materialSpecular[0] = 0.628281; materialSpecular[1] = 0.555802; materialSpecular[2] = 0.366065;
	materialShine = 0.4 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// ||silver 12 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 5, cHeight / 6 * 3, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.19225; materialAmbient[1] = 0.19225; materialAmbient[2] = 0.19225;
	materialDiffuse[0] = 0.50754; materialDiffuse[1] = 0.50754; materialDiffuse[2] = 0.50754;
	materialSpecular[0] = 0.508273; materialSpecular[1] = 0.508273; materialSpecular[2] = 0.508273;
	materialShine = 0.4 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// 3rd Coloumn
	//////////////////////////////////////////////////////////
	// black  13
	//////////////////////////////////////////////////////////

	gl.viewport(0, cHeight / 6 * 2, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.0; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.01; materialDiffuse[1] = 0.01; materialDiffuse[2] = 0.01;
	materialSpecular[0] = 0.50; materialSpecular[1] = 0.50; materialSpecular[2] = 0.50;
	materialShine = 0.25 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || pearl 4 Sphere||----------------------------------------------------------------------


	gl.viewport(cWidth / 6, cHeight / 6 * 2, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.1; materialAmbient[2] = 0.06;
	materialDiffuse[0] = 0.0; materialDiffuse[1] = 0.50980392; materialDiffuse[2] = 0.50980392;
	materialSpecular[0] = 0.50196078; materialSpecular[1] = 0.50196078; materialSpecular[2] = 0.50196078;
	materialShine = 0.25 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// || green 15 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 2, cHeight / 6 * 2, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.0; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.1; materialDiffuse[1] = 0.35; materialDiffuse[2] = 0.1;
	materialSpecular[0] = 0.45; materialSpecular[1] = 0.55; materialSpecular[2] = 0.45;
	materialShine = 0.25 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// || red 16 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 3, cHeight / 6 * 2, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.0; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.5; materialDiffuse[1] = 0.0; materialDiffuse[2] = 0.0;
	materialSpecular[0] = 0.7; materialSpecular[1] = 0.6; materialSpecular[2] = 0.6;
	materialShine = 0.25 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || pearl 4 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 4, cHeight / 6 * 2, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.0; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.55; materialDiffuse[1] = 0.55; materialDiffuse[2] = 0.55;
	materialSpecular[0] = 0.70; materialSpecular[1] = 0.70; materialSpecular[2] = 0.70;
	materialShine = 0.25 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || yellow plastic 18 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 5, cHeight / 6 * 2, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.0; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.5; materialDiffuse[1] = 0.5; materialDiffuse[2] = 0.0;
	materialSpecular[0] = 0.6; materialSpecular[1] = 0.6; materialSpecular[2] = 0.5;
	materialShine = 0.25 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	/// 4th coloumn
	//////////////////////////////////////////////////////////
	// black 19
	//////////////////////////////////////////////////////////gl.useProgram(shaderProgramObject);

	gl.viewport(0, cHeight / 6, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.02; materialAmbient[1] = 0.02; materialAmbient[2] = 0.02;
	materialDiffuse[0] = 0.01; materialDiffuse[1] = 0.01; materialDiffuse[2] = 0.01;
	materialSpecular[0] = 0.4; materialSpecular[1] = 0.4; materialSpecular[2] = 0.4;
	materialShine = 0.078125 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// || cyan 20 Sphere||----------------------------------------------------------------------
	gl.viewport(cWidth / 6, cHeight / 6, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.05; materialAmbient[2] = 0.05;
	materialDiffuse[0] = 0.4; materialDiffuse[1] = 0.5; materialDiffuse[2] = 0.5;
	materialSpecular[0] = 0.04; materialSpecular[1] = 0.7; materialSpecular[2] = 0.7;
	materialShine = 0.078125 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	
	// || green 21 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 2, cHeight / 6, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.0; materialAmbient[1] = 0.05; materialAmbient[2] = 0.05;
	materialDiffuse[0] = 0.4; materialDiffuse[1] = 0.5; materialDiffuse[2] = 0.4;
	materialSpecular[0] = 0.04; materialSpecular[1] = 0.7; materialSpecular[2] = 0.04;
	materialShine = 0.078125 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// || red 22 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 3, cHeight / 6, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.05; materialAmbient[1] = 0.0; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.5; materialDiffuse[1] = 0.4; materialDiffuse[2] = 0.4;
	materialSpecular[0] = 0.7; materialSpecular[1] = 0.04; materialSpecular[2] = 0.04;
	materialShine = 0.078125 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);

	// || white 23 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 4, cHeight / 6, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.05; materialAmbient[1] = 0.05; materialAmbient[2] = 0.05;
	materialDiffuse[0] = 0.5; materialDiffuse[1] = 0.5; materialDiffuse[2] = 0.5;
	materialSpecular[0] = 0.7; materialSpecular[1] = 0.7; materialSpecular[2] = 0.7;
	materialShine = 0.078125 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);
	

	// ||  yellow rubber  24 Sphere||----------------------------------------------------------------------

	gl.viewport(cWidth / 6 * 5, cHeight / 6, cWidth / 6, cHeight / 6);
	var modelMatrix = mat4.create();
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -2.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	materialAmbient[0] = 0.05; materialAmbient[1] = 0.05; materialAmbient[2] = 0.0;
	materialDiffuse[0] = 0.5; materialDiffuse[1] = 0.5; materialDiffuse[2] = 0.4;
	materialSpecular[0] = 0.7; materialSpecular[1] = 0.7; materialSpecular[2] = 0.04;
	materialShine = 0.078125 * 128.0;
	gl.uniform3fv(KdUniform, materialDiffuse);
	gl.uniform3fv(KaUniform, materialAmbient);
	gl.uniform3fv(KsUniform, materialSpecular);
	gl.uniform1f(MaterialShineUniform, materialShine);

	gl.bindVertexArray(vao_sphere);
	gl.drawArrays(gl.TRIANGLES, 0, sphere_vertices.length / 3);
	gl.bindVertexArray(null);


	gl.useProgram(null);

	if (bAnimateSpheres == true) {
		if (xRotation == 1) {
		LightPosition[0] = 0.0;
		LightPosition[1] = 7.0 * Math.sin(lightRotateAngleX);
		LightPosition[2] = 7.0 * Math.cos(lightRotateAngleX);
		LightPosition[3] = 0.0;

		lightRotateAngleX = lightRotateAngleX + 0.01;
		}
		else if (xRotation == 2) {
			LightPosition[0] = 7.0 * Math.sin(lightRotateAngleX);
			LightPosition[1] = 0.0;
			LightPosition[2] = 7.0 * Math.cos(lightRotateAngleX);
			LightPosition[3] = 0.0;

			lightRotateAngleX = lightRotateAngleX + 0.01;
		}
		else if (xRotation == 3) {
			LightPosition[0] = 7.0 * Math.sin(lightRotateAngleX);
			LightPosition[1] = 7.0 * Math.cos(lightRotateAngleX);
			LightPosition[2] = 0.0;
			LightPosition[3] = 0.0;

			lightRotateAngleX = lightRotateAngleX + 0.01;
		}
	}

	requestAnimationFrame(draw, canvas);
}


function uninitialize()
{
	if (shaderProgramObject)
	{
		if (vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		if (fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
}
