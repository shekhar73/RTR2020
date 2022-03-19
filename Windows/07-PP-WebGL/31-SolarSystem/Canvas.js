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

var vao;
var vbo_position;
var vbo_normal;
var vbo_element;

var sphereVertices;

var gNumSphereVertices;
var gNumSphereElements;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var perspectiveProjectionMatrix;

var rotationOfEarth = 0.0;
var rotationOfEarthItself = 0.0;


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
	resize();	// warmpu REPAINT call due to below draw()
	draw();
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
		case 70:
            toggleFullscreen();
            break;

		case 27:
			UnInitialize();
			window.close();
			break;

		case 83:
			shoulderRotate = shoulderRotate + 0.5;
			break;

		case 69:
			elbowRotate = elbowRotate + 0.5;
			break;

		default:
			break;
	}
}

function init()
{
	// get drawing gl
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
		"in vec4 vPosition;" +
		"uniform mat4 u_modelMatrix;" +
		"uniform mat4 u_viewMatrix;" +
		"uniform mat4 u_projectionMatrix;" +
		"void main(void)" +
		"{" +
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
			UnInitialize();
        }
	}

	// fragment shader
	var fragmentShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"FragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
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

	// shader program
	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	 //pre-link binding of shader program object with vertex shader attributes

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_COLOR, "vColor");

	 // linking

	gl.linkProgram(shaderProgramObject);
	if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if (error.length > 0)
		{
			alert(error);
			UnInitialize();
		}
	}

	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projectionMatrix");

	sphereVertices = invokeVerticesOfSphere(0.5, 40, 40);

	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, sphereVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);
	
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearDepth(1.0);
	
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();
}

function invokeVerticesOfSphere(radius, lats, longs) {
	var vertices = Array();

	const rad = Math.PI / 180.0;
	const latitudesValues = 180.0 / lats;
	const longitudesValues = 360.0 / longs;

	for (var i = 0.0; i < 360.0; i += longitudesValues) 
	{
		var iNextValue = i + longitudesValues;
		for (var j = 0.0; j < 180.0; j += latitudesValues)
		 {
			var jNextValues = j + latitudesValues;

			var points = Array(4);
			var x, y, z;

			x = radius * Math.cos(i * rad) * Math.sin(j * rad);
			y = radius * Math.cos(j * rad);
			z = radius * Math.sin(i * rad) * Math.sin(j * rad);
			points[0] = Array(x, y, z);

			x = radius * Math.cos(iNextValue * rad) * Math.sin(j * rad);
			y = radius * Math.cos(j * rad);
			z = radius * Math.sin(iNextValue * rad) * Math.sin(j * rad);
			points[1] = Array(x, y, z);

			x = radius * Math.cos(iNextValue * rad) * Math.sin(jNextValues * rad);
			y = radius * Math.cos(jNextValues * rad);
			z = radius * Math.sin(iNextValue * rad) * Math.sin(jNextValues * rad);
			points[2] = Array(x, y, z);

			x = radius * Math.cos(i * rad) * Math.sin(jNextValues * rad);
			y = radius * Math.cos(jNextValues * rad);
			z = radius * Math.sin(i * rad) * Math.sin(jNextValues * rad);
			points[3] = Array(x, y, z);

			var triangleOne = points[0].concat(points[1], points[2]);
			var triangleTwo = points[2].concat(points[3], points[0]);

			vertices = vertices.concat(triangleOne, triangleTwo);
		}
	}
	return Float32Array.from(vertices);
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

const STACK_OF_MATRIX_MAXIMUM = 32;
var stackOfMatrix = [];
var iCounter = -1;

function pushInToMatrix(matrix) 
{
	if (iCounter < STACK_OF_MATRIX_MAXIMUM - 1) 
	{
		iCounter++;
		if (!stackOfMatrix[iCounter]) {
			stackOfMatrix[iCounter] = [];
		}
		for (let i = 0; i < 16; i++) {
			stackOfMatrix[iCounter][i] = matrix[i];
		}
	}
	else 
	{
		console.log("Stack Overflow");
	}
}

function popFromMatrix() {
	var val = [];

	if (iCounter > -1) {

		for (let i = 0; i < 16; i++) 
		{
			val[i] = stackOfMatrix[iCounter][i];
		}

		iCounter--
		stackOfMatrix.pop();

		return val;
	}
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var translateMatrix = mat4.create();
	var scaleMatrix = mat4.create();
	var rotateMatrix = mat4.create();


	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0, -12.0]);
	mat4.multiply(modelMatrix, modelMatrix, translateMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	pushInToMatrix(mat4.clone(modelMatrix));

	mat4.rotate(modelMatrix, modelMatrix, convertDegToRad(rotationOfEarthItself), [0.0, 1.0, 0.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

	pushInToMatrix(mat4.clone(modelMatrix));
	mat4.scale(scaleMatrix, scaleMatrix, [3.0, 3.0, 3.0]);
	mat4.multiply(modelMatrix, modelMatrix, scaleMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.bindVertexArray(vao);
	gl.drawArrays(gl.TRIANGLES, 0, sphereVertices.length / 3);
	gl.bindVertexArray(null);
	modelMatrix = popFromMatrix();

	translateMatrix = mat4.create();
	mat4.translate(translateMatrix, translateMatrix, [4.5, 0.0, 0.0]);
	mat4.multiply(modelMatrix, modelMatrix, translateMatrix);

	rotateMatrix = mat4.create();
	mat4.rotate(modelMatrix, modelMatrix, convertDegToRad(rotationOfEarth), [0.0, 1.0, 0.0]);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

	pushInToMatrix(modelMatrix);
	scaleMatrix = mat4.create();
	mat4.scale(scaleMatrix, scaleMatrix, [1.0, 1.0, 1.0]);
	mat4.multiply(modelMatrix, modelMatrix, scaleMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.bindVertexArray(vao);
	gl.drawArrays(gl.TRIANGLES, 0, sphereVertices.length / 3);
	gl.bindVertexArray(null);
	
	modelViewMatrix = popFromMatrix();
	modelViewMatrix = popFromMatrix();
	gl.useProgram(null);
	requestAnimationFrame(draw, canvas);
}

function convertDegToRad(degree) 
{
	return rad = (degree * Math.PI) / 180.0;
}

function UnInitialize()
{
	if (vao)
	{
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if (vbo_vertices)
	{
		gl.deleteBuffer(vbo_vertices);
		vbo_vertices = null;
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
