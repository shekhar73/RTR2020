
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

var vao_cube;
var vbo_cube;

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

var bAnimate = false;
var bLight = false;
var angle_cube = 0.0;

var cube_texture;
var textureSamplerUniform;

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
	// get Canvas from Doc
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
	switch (event.keyCode)
	{
		case 70:
            toggleFullscreen();
            break;

		case 27:
			UnInitialize();
			window.close();
			break;

		case 76:
		case 108:
			if(bLight == false)
				bLight = true;
			else
				bLight = false;
			break;

		default:
			break;
	}
}


function mouseDown()
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
	var vertexShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"in vec4 vColor;" +
		"in vec2 vTexCoord;" +
		"uniform mat4 u_modelMatrix;" +
		"uniform mat4 u_viewMatrix;" +
		"uniform mat4 u_projectionMatrix;" +
		"uniform int u_lKeyPressed;" +
		"uniform vec4 light_position;" +
		"out vec3 transformed_Normal;" +
		"out vec3 light_direction;" +
		"out vec3 view_vector;" +
		"out vec2 out_texCoord;" +
		"out vec4 out_Color;" +
		"void main(void)" +
		"{" +
		"if(u_lKeyPressed == 1){" +
		"vec4 eyeCoordinate = u_viewMatrix * u_modelMatrix * vPosition;" +
		"transformed_Normal = mat3(u_viewMatrix * u_modelMatrix) * vNormal;" +
		"light_direction = vec3(light_position - eyeCoordinate);" +
		"view_vector = -eyeCoordinate.xyz;" +
		"}" +
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vPosition;" +
		"out_texCoord = vTexCoord;" +
		"out_Color = vColor;" +
		"}";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	// provide source code to shader
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);

	// compile shader & check for errors
	gl.compileShader(vertexShaderObject);

	if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if (error.length > 0)
		{
			alert(error);
			UnInitialize();
        }
	}

	/* Fragment Shader */
	var fragmentShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"vec3 phong_ads_light;" +
		"in vec3 transformed_Normal;" +
		"in vec3 light_direction;" +
		"in vec3 view_vector;" +
		"in vec2 out_texCoord;" +
		"in vec4 out_Color;" +
		"out vec4 FragColor;" +
		"uniform sampler2D u_texture_sampler;" +
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
		"FragColor = texture(u_texture_sampler,out_texCoord) * vec4(phong_ads_light, 1.0) * out_Color;" +
		"}";

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	
	if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if (error.length > 0)
		{
			alert(error);
			UnInitialize();
		}
	}

	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_NORMAL, "vNormal");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_TEXTURE, "vTexCoord");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_COLOR, "vColor");

	gl.linkProgram(shaderProgramObject);
	if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if (error.length > 0)
		{
			alert(error);
			UnInitialize();
		}
	}

	textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_sampler");
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

	const cube = new Float32Array([
		
		 1.0, 1.0, 1.0, 	1.0, 0.0, 0.0, 	0.0, 0.0, 1.0, 	1.0, 0.0,
		-1.0, 1.0, 1.0, 	1.0, 0.0, 0.0, 	0.0, 0.0, 1.0, 	0.0, 0.0,
		-1.0, -1.0, 1.0, 	1.0, 0.0, 0.0, 	0.0, 0.0, 1.0, 	0.0, 1.0,
		 1.0, -1.0, 1.0, 	1.0, 0.0, 0.0, 	0.0, 0.0, 1.0, 	1.0, 1.0,
		
		1.0, 1.0, -1.0, 	0.0, 1.0, 0.0, 	1.0, 0.0, 0.0, 	1.0, 0.0,
		1.0, 1.0, 1.0, 		0.0, 1.0, 0.0, 	1.0, 0.0, 0.0, 	0.0, 0.0,
		1.0, -1.0, 1.0, 	0.0, 1.0, 0.0, 	1.0, 0.0, 0.0, 	0.0, 1.0,
		1.0, -1.0, -1.0, 	0.0, 1.0, 0.0, 	1.0, 0.0, 0.0, 	1.0, 1.0,
	
		-1.0, 1.0, -1.0, 	0.0, 0.0, 1.0, 	0.0, 0.0, -1.0, 	1.0, 0.0,
		1.0, 1.0, -1.0, 	0.0, 0.0, 1.0, 	0.0, 0.0, -1.0, 	0.0, 0.0,
		1.0, -1.0, -1.0, 	0.0, 0.0, 1.0, 	0.0, 0.0, -1.0, 	0.0, 1.0,
		-1.0, -1.0, -1.0, 	0.0, 0.0, 1.0, 	0.0, 0.0, -1.0, 	1.0, 1.0,
		
		-1.0, 1.0, 1.0, 	1.0, 0.5, 0.0, 	-1.0, 0.0, 0.0, 	1.0, 0.0,
		-1.0, 1.0, -1.0, 	1.0, 0.5, 0.0, 	-1.0, 0.0, 0.0, 	0.0, 0.0,
		-1.0, -1.0, -1.0, 	1.0, 0.5, 0.0, 	-1.0, 0.0, 0.0, 	0.0, 1.0,
		-1.0, -1.0, 1.0, 	1.0, 0.5, 0.0, 	-1.0, 0.0, 0.0, 	1.0, 1.0,
	
		1.0, 1.0, -1.0, 	0.5, 0.5, 1.0, 	0.0, 1.0, 0.0, 		1.0, 0.0,
		-1.0, 1.0, -1.0, 	0.5, 0.5, 1.0, 	0.0, 1.0, 0.0, 		0.0, 0.0,
		-1.0, 1.0, 1.0, 	0.5, 0.5, 1.0, 	0.0, 1.0, 0.0, 		0.0, 1.0,
		1.0, 1.0, 1.0, 		0.5, 0.5, 1.0, 	0.0, 1.0, 0.0, 		1.0, 1.0,
		
		-1.0, -1.0, -1.0, 	1.0, 1.0, 1.0, 	0.0, -1.0, 0.0, 	1.0, 0.0,
		1.0, -1.0, -1.0, 	1.0, 1.0, 1.0, 	0.0, -1.0, 0.0, 	0.0, 0.0,
		1.0, -1.0, 1.0, 	1.0, 1.0, 1.0, 	0.0, -1.0, 0.0, 	0.0, 1.0,
		-1.0, -1.0, 1.0, 	1.0, 1.0, 1.0, 	0.0, -1.0, 0.0, 	1.0, 1.0
	]);

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	vbo_cube = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube);
	gl.bufferData(gl.ARRAY_BUFFER, cube, gl.STATIC_DRAW);

	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 11 * 4, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);

	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 11 * 4, 12);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);

	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 11 * 4, 24);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_NORMAL);

	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE, 2, gl.FLOAT, false, 11 * 4, 36);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);


	gl.bindVertexArray(null);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);


	cube_texture = gl.createTexture();   
	cube_texture.image = new Image();
	cube_texture.image.src = "marble.png";

	cube_texture.image.onload = function () {
		gl.bindTexture(gl.TEXTURE_2D, cube_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, cube_texture.image);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}

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
	var materialAmbient = new Float32Array([0.0, 0.0, 0.0]);
	var materialDiffuse = new Float32Array([0.5, 0.2, 0.7]);
	var materialSpecular = new Float32Array([0.7, 0.7, 0.7]);

	var materialShine = 128.0;

	var LightAmbient = new Float32Array([0.1, 0.1, 0.1]);
	var LightDiffuse = new Float32Array([1.0, 1.0, 1.0]);
	var LightSpecular = new Float32Array([1.0, 1.0, 1.0]);
	var LightPosition = new Float32Array([100.0, 100.0, 100.0, 1.0]);


	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	gl.uniform3fv(LightAmbientUniform, LightAmbient);
	gl.uniform3fv(LightDiffuseUniform, LightDiffuse);
	gl.uniform3fv(LightSpecularUniform, LightSpecular);
	gl.uniform4fv(LightPositionUniform, LightPosition);

	if (bLight == true) 
	{
		gl.uniform1i(lKeyPressedUniform, 1);
		gl.uniform3fv(KdUniform, materialDiffuse);
		gl.uniform3fv(KaUniform, materialAmbient);
		gl.uniform3fv(KsUniform, materialSpecular);
		gl.uniform1f(MaterialShineUniform, materialShine);
	}
	else 
	{
		gl.uniform1i(lKeyPressedUniform, 0);
	}

	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -4.0]);
	mat4.rotate(modelMatrix, modelMatrix, degToRad(angle_cube), [1.0, 0.0, 0.0]);
	mat4.rotate(modelMatrix, modelMatrix, degToRad(angle_cube), [0.0, 1.0, 0.0]);
	mat4.rotateZ(modelMatrix, modelMatrix, degToRad(angle_cube));
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(perspectiveProjectionUniform, false, perspectiveProjectionMatrix);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, cube_texture);
	gl.uniform1i(textureSamplerUniform, 0);

	gl.bindVertexArray(vao_cube);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	gl.bindVertexArray(null);


	gl.useProgram(null);

	if (bAnimate == true) 
	{
		if (angle_cube <= 360.0) 
		{
			angle_cube = angle_cube + 0.1;
		}
		else 
		{
			angle_cube = 0.0;
		}
	}

	requestAnimationFrame(draw, canvas);
}

function degToRad(degree) 
{
	return (degree * Math.PI) / 180.0;
}


function UnInitialize()
{
	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube = null;
	}

	if(vbo_PCNT_cube)
	{
		gl.deleteVertexArray(vbo_PCNT_cube);
		vbo_PCNT_cube = null;
	}
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



