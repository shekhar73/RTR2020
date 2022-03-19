
var canvas = null;
var gl = null;	// webgl context
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

var angle_pyramid = 0.0;
var angle_cube = 0.0;

const WebGLMacros = 
{
	SSK_ATTRIBUTE_POSITION:0,
    SSK_ATTRIBUTE_COLOR:1,
    SSK_ATTRIBUTE_NORMAL:2,
    SSK_ATTRIBUTE_TEXTURE0:3,

};

// Shaders
var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

// Pyramid
var vao_pyramid;
var vbo_texture_pyramid;
var vbo_Position_pyramid;


// Cube
var vao_cube;
var vbo_texture_cube;
var vbo_Position_cube;


// Texture
var pyramid_stone_texture;
var cube_kundali_texture;
var gTextureSamplerUniform;

var mvpUniform;
var perspeciveProjectionMatrix;
  
var canvas = document.getElementById("SSK");
gl = canvas.getContext("webgl2");

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
	"in vec2 vTexCoord;"+
	"uniform mat4 u_mvp_matrix;"+
	"out vec2 out_TexCoord;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
	"out_TexCoord = vTexCoord;"+
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
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec2 out_TexCoord;"+
	"uniform sampler2D u_texture_sampler;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = texture(u_texture_sampler, out_TexCoord);"+
	"}";

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

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, "vTexCoord");


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

	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");
	gTextureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_sampler");

	var pyramidVertices = new Float32Array([
												
								0.0,0.5,0.0,
								-0.5,-0.5,0.5,
								 0.5,-0.5,0.5,

								0.0,0.5,0.0,
								0.5,-0.5,0.5,
								0.5,-0.5,-0.5,

								0.0,0.5,0.0,
								0.5,-0.5,-0.5,
								-0.5,-0.5,-0.5,

								0.0,0.5,0.0,
								-0.5,-0.5,-0.5,
								-0.5,-0.5,0.5
	]);

	var texturePyramid = new Float32Array([
										0.5, 1.0,
										0.0, 0.0,
										1.0, 0.0,

										0.5, 1.0,
										1.0, 0.0,
										0.0, 0.0,

										0.5, 1.0,
										0.0, 0.0,
										1.0, 0.0,

										0.5, 1.0,
										1.0, 0.0,
										0.0, 0.0 
	]);


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

	var cubeColors = new Float32Array 
            ([
				0.0, 0.0,
                1.0, 0.0,
                1.0,1.0,
                0.0,1.0,

                //RIGHT ACE
                1.0, 0.0,
                1.0,1.0,
                0.0,1.0,
                0.0, 0.0,

                //BACK ACE
                1.0, 0.0,
                1.0, 1.0,
                0.0, 1.0,
                0.0, 0.0,

                //LET ACE
                0.0, 0.0,
                1.0, 0.0,
                1.0, 1.0,
                0.0, 1.0,

                //TOP ACE
                0.0,1.0,
                0.0, 0.0,
                1.0, 0.0,
                1.0, 1.0,

                //BOTTOM ACE
                1.0, 1.0,
                0.0, 1.0,
                0.0, 0.0,
                1.0, 0.0
            ]);

	vao_pyramid = gl.createVertexArray();

	gl.bindVertexArray(vao_pyramid);
		vbo_Position_pyramid = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_Position_pyramid);
		gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		
		vbo_texture_pyramid = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture_pyramid);
		gl.bufferData(gl.ARRAY_BUFFER, texturePyramid, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);


	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
		vbo_Position_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_Position_cube);
		gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vbo_texture_cube = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture_cube);
		gl.bufferData(gl.ARRAY_BUFFER,cubeColors,gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);


	// Like LoadGLTexture();
	pyramid_stone_texture = gl.createTexture();
	pyramid_stone_texture.image = new Image();
	pyramid_stone_texture.image.src = "stone.png";

	// lambda/closure/block/unnamed block
	pyramid_stone_texture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, pyramid_stone_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

		// push data into graphic memory with the help of graphics driver
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, pyramid_stone_texture.image);

		gl.bindTexture(gl.TEXTURE_2D, null);
	};



	cube_kundali_texture = gl.createTexture();
	cube_kundali_texture.image = new Image();
	cube_kundali_texture.image.src = "kundali.png";
	cube_kundali_texture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, cube_kundali_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, cube_kundali_texture.image);
		gl.bindTexture(gl.TEXTURE_2D, null);
	};

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
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -3.0]);
	mat4.scale(modelViewMatrix, modelViewMatrix, [0.75, 0.75, 0.75]);

	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle_cube));
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_cube));
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(angle_cube));
	

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);


	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, cube_kundali_texture);
	gl.uniform1i(gTextureSamplerUniform, 0);

	gl.bindVertexArray(vao_cube);

	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	
	gl.bindVertexArray(null);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -3.0]);

	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle_pyramid));

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, pyramid_stone_texture);
	gl.uniform1i(gTextureSamplerUniform, 0)

	gl.bindVertexArray(vao_pyramid);

	gl.drawArrays(gl.TRIANGLES, 0, 12);

	gl.bindVertexArray(null);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	gl.useProgram(null);

	gl.useProgram(null);

	angle_cube = angle_cube + 1.0;
	if(angle_cube >= 360.0)
	{
		angle_cube = 0.0;
	}

	angle_pyramid = angle_pyramid + 1.0;
	if(angle_pyramid >= 360.0)
	{
		angle_pyramid = 0.0;
	}

	requestAnimationFrame(draw, canvas);
}

function degToRad(degree)
{
	return(degree * Math.PI / 180.0);
}

function uninitialize()
{
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}

	if(vbo_Position_pyramid)
	{
		gl.deleteBuffer(vbo_Position_pyramid);
		vbo_Position_pyramid = null;
	}

	if(vbo_texture_pyramid)
	{
		gl.deleteBuffer(vbo_texture_pyramid);
		vbo_texture_pyramid = null;
	}

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