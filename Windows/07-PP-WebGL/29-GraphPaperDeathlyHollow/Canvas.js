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


var gVao_Graph;
var gVbo_Graph_Vertex;
var gVbo_Graph_Color;

var gVao_Axis;
var gVbo_Axis_Vertex;
var gVbo_Axis_Color;

var gVao_Rectangle;
var gVbo_Rectangle_Vertex;
var gVbo_Rectangle_Color;

var gVao_Triangle;
var gVbo_Triangle_Vertex;
var gVbo_Triangle_Color;

var gVao_First_Circle;
var gVbo_First_Circle_Vertex;
var gVbo_First_Circle_Color;

var gVao_Second_Circle;
var gVbo_Second_Circle_Vertex;
var gVbo_Second_Circle_Color;

var gCenterX;
var gCenterY;

var mvpMatrixUniform;



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
	"in vec4 vColor;"+
	"uniform mat4 u_mvp_matrix;"+
	"out vec4 out_color;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
	"out_color = vColor;"+
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
	"in vec4 out_color;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = out_color;"+
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_COLOR, "vColor");

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
	mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");

	 // *** vertices, colors, shader attribs, vbo, vao initializations *** 
	 
	//---------------------GRAPH---------------------------------------------------------

	var graphVertices = [42 * 2 * 3 * 2];
	var x = -1.0;

	for (let i = 0; i < 41 * 2 * 3; i = i + 6)
	{
		graphVertices[i] = x;
		graphVertices[i + 1] = 1.0;
		graphVertices[i + 2] = 0.0;

		graphVertices[i + 3] = x;
		graphVertices[i + 4] = -1.0;
		graphVertices[i + 5] = 0.0;

		x = x + 0.05;
	}

	var y = 1.0;

	for (let i = 41 * 2 * 3; i < 41 * 2 * 3 * 2; i = i + 6)
	{
		graphVertices[i] = -1.0;
		graphVertices[i + 1] = y;
		graphVertices[i + 2] = 0.0;

		graphVertices[i + 3] = 1.0;
		graphVertices[i + 4] = y;
		graphVertices[i + 5] = 0.0;

		y = y - 0.05;
	}

	var graphColors = [42 * 2 * 3 * 2];
	for (let i = 0; i < 42 * 2 * 3 * 2; i = i + 3)
	{
		graphColors[i] = 0.0;
		graphColors[i + 1] = 0.0;
		graphColors[i + 2] = 1.0;
	}


	/*var triangleVertices = new Float32Array([
	 											0.0, 1.0, 0.0,		// appex
	 											-1.0, -1.0, 0.0, 	// left-bottom
	 											1.0, -1.0, 0.0 	// right-bottom
 	]);*/

	gVao_Graph = gl.createVertexArray();
	gl.bindVertexArray(gVao_Graph);

	gVbo_Graph_Vertex = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Graph_Vertex);
	gl.bufferData(gl.ARRAY_BUFFER, graphVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gVbo_Graph_Color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Graph_Color);
	gl.bufferData(gl.ARRAY_BUFFER, graphColors, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

//---------------------End of GRAPH---------------------------------------------------------


//----------------- AXIS of X and Y---------------------------------------------------------
	
	var AxisVertices = new Float32Array([
	 	-1.0,	0.0,	0.0,
		 1.0,	0.0,	0.0,
		 0.0,	1.0,	0.0,
		 0.0, 	-1.0,	0.0
 	]);

 	var AxisColors = new Float32Array([
	 	1.0,	0.0,	0.0,
		1.0,	0.0,	0.0,
		0.0,	1.0,	0.0,
		0.0,	1.0,	0.0,
 	]);

 	gVao_Axis = gl.createVertexArray();
	gl.bindVertexArray(gVao_Axis);

	gVbo_Axis_Vertex = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Axis_Vertex);
	gl.bufferData(gl.ARRAY_BUFFER, AxisVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gVbo_Axis_Color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Axis_Color);
	gl.bufferData(gl.ARRAY_BUFFER, AxisColors, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

//----------------- End of AXIS of X and Y-----------------------------------------------


//------------------Rectangle------------------------------------------------------------


	var rectVertices = new Float32Array([
	 	-1.0,	0.75,	0.0,
		-1.0, -0.75,	0.0,

		-1.0, -0.75,	0.0,
		 1.0,  -0.75,	0.0,

		 1.0,  -0.75,	0.0,
		 1.0,   0.75,	0.0,

		 1.0,	0.75,	0.0,
		-1.0,	0.75,	0.0
 	]);

 	var rectColors = new Float32Array([
	 	1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0
 	]);

 	gVao_Rectangle = gl.createVertexArray();
	gl.bindVertexArray(gVao_Rectangle);

	gVbo_Rectangle_Vertex = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Rectangle_Vertex);
	gl.bufferData(gl.ARRAY_BUFFER, rectVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gVbo_Rectangle_Color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Rectangle_Color);
	gl.bufferData(gl.ARRAY_BUFFER, rectColors, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

//------------------End of Rectangle------------------------------------------------------------

//------------------Triangle------------------------------------------------------------
	
	var x1 = 0.0;
	var y1 = 0.75;

	var x2 = (Math.cos(120.0) * 0.75);
	var y2 = -(Math.sin(120.0) * 0.75);

	var x3 = -(Math.cos(120.0) * 0.75);
	var y3 = -(Math.sin(120.0) * 0.75);

	var triangleVertices = new Float32Array([
	 	x1, y1, 0.0,
		x2, y2, 0.0,

		x2, y2, 0.0,
		x3, y3, 0.0,

		x3, y3, 0.0,
		x1, y1, 0.0,
 	]);

 	var triangleColors = new Float32Array([
	 	0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0
 	]);

 	gVao_Triangle = gl.createVertexArray();
	gl.bindVertexArray(gVao_Triangle);

	gVbo_Triangle_Vertex = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Triangle_Vertex);
	gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gVbo_Triangle_Color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Triangle_Color);
	gl.bufferData(gl.ARRAY_BUFFER, triangleColors, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

	//------------------End of Triangle------------------------------------------------------------


	//------------------Circle------------------------------------------------------------
	var radius = 0.75;
	var circle1Vertices = [360 * 9];
	var circle1Colors = [360 * 9];

	var angle = 0.0;
	for (let i = 0; i < 360 * 9; i = i + 3)
	{
		circle1Vertices[i] = radius * Math.cos(angle);
		circle1Vertices[i + 1] = radius * Math.sin(angle);
		circle1Vertices[i + 2] = 0.0;

		angle = angle + 0.006;

		circle1Colors[i] = 1.0;
		circle1Colors[i + 1] = 0.05;
		circle1Colors[i + 2] = 1.0;
	}

	gVao_First_Circle = gl.createVertexArray();
	gl.bindVertexArray(gVao_First_Circle);

	gVbo_First_Circle_Vertex = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_First_Circle_Vertex);
	gl.bufferData(gl.ARRAY_BUFFER, circle1Vertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gVbo_First_Circle_Color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_First_Circle_Color);
	gl.bufferData(gl.ARRAY_BUFFER, circle1Colors, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);

	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);


	var a = Math.sqrt(((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3)));
	var b = Math.sqrt(((x1 - x3) * (x1 - x3)) + ((y1 - y3) * (y1 - y3)));
	var c = Math.sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
	
	gCenterX = ((a * x1) + (b * x2) + (c * x3)) / (a + b + c);
	gCenterY = ((a * y1) + (b * y2) + (c * y3)) / (a + b + c);
	
	var Perimeter = (a + b + c) / 2;
	var Area = Math.sqrt(Perimeter * (Perimeter - a) * (Perimeter - b) * (Perimeter - c));
	var radiusInCircle = Area / Perimeter;

	var circle2Vertices = [360 * 9];
	var circle2Colors = [360 * 9];

	var angle = 0.0;
	for (let i = 0; i < 360 * 9; i = i + 3)
	{
		circle2Vertices[i] = radiusInCircle * Math.cos(angle);
		circle2Vertices[i + 1] = radiusInCircle * Math.sin(angle);
		circle2Vertices[i + 2] = 0.0;

		angle = angle + 0.006;

		circle2Colors[i] = 1.5;
		circle2Colors[i + 1] = 0.05;
		circle2Colors[i + 2] = 0.0;
	}

	gVao_Second_Circle = gl.createVertexArray();
	gl.bindVertexArray(gVao_Second_Circle);

	gVbo_Second_Circle_Vertex = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Second_Circle_Vertex);
	gl.bufferData(gl.ARRAY_BUFFER, circle2Vertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gVbo_Second_Circle_Color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, gVbo_Second_Circle_Color);
	gl.bufferData(gl.ARRAY_BUFFER, circle2Colors, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
		3, // 3 is for X, Y, Z co-ordinates in our triangleVertices array
		gl.FLOAT,
		false, 0, 0);

	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
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
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrx = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);

	mat4.multiply(modelViewProjectionMatrx, perspectiveProjectionMatrix, modelViewMatrix);
	
	gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrx);

	//--------------------- Graph------------------------------------------------------
	gl.bindVertexArray(gVao_Graph);

	// 41 lines in X direction and 41 lines in Y direction
	for (let i = 0; i < 41 * 2; i++)
	{
		gl.drawArrays(gl.LINES, i * 2, 2);
	}
	gl.bindVertexArray(null);
	//------------------ End of graph----------------------------------------------------


	//--------------------- Draw Axises------------------------------------------------------
	gl.bindVertexArray(gVao_Axis);
		// X- Axis
		gl.drawArrays(gl.LINES, 0, 2);
		// Y- Axis
		gl.drawArrays(gl.LINES, 2, 2);
	gl.bindVertexArray(null);
	//------------------ End of Axises----------------------------------------------------

	//--------------------- Draw Rectangle--------------------------------------------------------
	gl.bindVertexArray(gVao_Rectangle);
	for (let i = 0; i < 8; i++)
	{
		gl.drawArrays(gl.LINES, i * 2, 2);
	}
	gl.bindVertexArray(null);
	//--------------------- End of Rectangle------------------------------------------------------

	//--------------------- Circle 1------------------------------------------------------
	gl.bindVertexArray(gVao_First_Circle);
	for(let i = 0; i < 360 * 9; i++)
	{
		gl.drawArrays(gl.POINTS, i, 1);
	}
	gl.bindVertexArray(null);
	//--------------------- End of Circle 1------------------------------------------------------

	//--------------------- Draw Triangle--------------------------------------------------------
	gl.bindVertexArray(gVao_Triangle);
	for (let i = 0; i < 6; i++)
	{
		gl.drawArrays(gl.LINES, i * 2, 2);
	}
	gl.bindVertexArray(null);
	//--------------------- End of Triangle------------------------------------------------------



	gl.useProgram(null);

	requestAnimationFrame(draw, canvas);

}

function uninitialize()
{
	// code
	if(gVao_Graph)
	{
		gl.deleteVertexArray(gVao_Graph);
		gVao_Graph = null;
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