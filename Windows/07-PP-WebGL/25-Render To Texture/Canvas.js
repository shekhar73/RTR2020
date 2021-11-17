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
var fragmentShaderObject_1;
var fragmentShaderObject_2;
var shaderProgramObject_1;
var shaderProgramObject_2;

var Width;
var Height;

var vao;
var vbo;
var fbo;

var position_buffer;
var index_buffer;

var color_texture;
var depth_texture;

var mvpUniform_1;
var mvpUniform_2;
var projectionUniform_1;
var projectionUniform_2;

var cube_angle = 0.0;

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

	// vertex shader
	var vertexShaderSourceCode = 
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;" +
	"in vec2 vTexcoord;" +
	"out vec4 out_color;" +
	"out vec2 out_texcoord;" +
	"uniform mat4 u_mv_matrix;" +
	"uniform mat4 u_projection_matrix;" +
	"void main(void)" +
	"{" +
		"gl_Position = u_projection_matrix * u_mv_matrix * vPosition;" +
		"out_color = vPosition * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);" +
		"out_texcoord = vTexcoord;" +
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	fragmentShaderObject_1 = gl.createShader(gl.FRAGMENT_SHADER);

	// fragment shader
	var fragmentShaderSourceCode_1 = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec4 out_color;"  +
	"in vec2 out_texcoord;" +
	"out vec4 FragColor;" +
	"void main(void)" +
	"{" +
	"FragColor = sin(out_color * vec4(40.0, 20.0, 30.0, 1.0)) * 0.5 + vec4(0.5);" +
	"}";

	gl.shaderSource(fragmentShaderObject_1, fragmentShaderSourceCode_1);
	gl.compileShader(fragmentShaderObject_1);

	if(gl.getShaderParameter(fragmentShaderObject_1, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject_1);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	fragmentShaderObject_2 = gl.createShader(gl.FRAGMENT_SHADER);

	// fragment shader
	var fragmentShaderSourceCode_2 = 
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"uniform sampler2D tex;" +
    "out vec4 FragColor;" +
    "in vec4 out_color;" +
    "in vec2 out_texcoord;" +
    "void main(void)" +
    "{" +
    	"FragColor = mix(out_color, texture(tex,out_texcoord), 0.7);" +
    "}";

	gl.shaderSource(fragmentShaderObject_2, fragmentShaderSourceCode_2);
	gl.compileShader(fragmentShaderObject_2);

	if(gl.getShaderParameter(fragmentShaderObject_2, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject_2);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// shader program
	shaderProgramObject_1 = gl.createProgram();

	gl.attachShader(shaderProgramObject_1, vertexShaderObject);
	gl.attachShader(shaderProgramObject_1, fragmentShaderObject_1);

	 //pre-link binding of shader program object with vertex shader attributes
	 gl.bindAttribLocation(shaderProgramObject_1, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	 gl.bindAttribLocation(shaderProgramObject_1, WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, "vTexcoord");

	 // linking
	 gl.linkProgram(shaderProgramObject_1);

	 if(gl.getProgramParameter(shaderProgramObject_1, gl.LINK_STATUS) == false)
	 {
	 	var error = gl.getProgramInfoLog(shaderProgramObject_1);
	 	if(error.length > 0)
	 	{
	 		alert(error);
	 		uninitialize();
	 	}
	 }

	 // get MVP uniform location
	 mvpUniform_1 = gl.getUniformLocation(shaderProgramObject_1, "u_mv_matrix");
	 projectionUniform_1 = gl.getUniformLocation(shaderProgramObject_1, "u_projection_matrix");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 
// shader program
	shaderProgramObject_2 = gl.createProgram();

	gl.attachShader(shaderProgramObject_2, vertexShaderObject);
	gl.attachShader(shaderProgramObject_2, fragmentShaderObject_2);

	 //pre-link binding of shader program object with vertex shader attributes
	 gl.bindAttribLocation(shaderProgramObject_2, WebGLMacros.SSK_ATTRIBUTE_POSITION, "vPosition");
	 gl.bindAttribLocation(shaderProgramObject_2, WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, "vTexcoord");

	 // linking
	 gl.linkProgram(shaderProgramObject_2);

	 if(gl.getProgramParameter(shaderProgramObject_2, gl.LINK_STATUS) == false)
	 {
	 	var error = gl.getProgramInfoLog(shaderProgramObject_2);
	 	if(error.length > 0)
	 	{
	 		alert(error);
	 		uninitialize();
	 	}
	 }

	 // get MVP uniform location
	 mvpUniform_2 = gl.getUniformLocation(shaderProgramObject_2, "u_mv_matrix");
	 projectionUniform_2 = gl.getUniformLocation(shaderProgramObject_2, "u_projection_matrix");
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	 var vertex_indices = new Float32Array
	 ([
	 		0, 1, 2,
            2, 1, 3,
            2, 3, 4,
            4, 3, 5,
            4, 5, 6,
            6, 5, 7,
            6, 7, 0,
            0, 7, 1,
            6, 0, 2,
            2, 4, 6,
            7, 5, 3,
            7, 3, 1
	 ]);

	 var vertex_cube = new Float32Array
	 ([
			// Position              
            -0.5, -0.5,  0.5,      
            -0.5, -0.5, -0.5,      
             0.5, -0.5, -0.5,      

             0.5, -0.5, -0.5,    
             0.5, -0.5,  0.5,      
            -0.5, -0.5,  0.5,      

             0.5, -0.5, -0.5,      
             0.5,  0.5, -0.5,      
             0.5, -0.5,  0.5,      

             0.5,  0.5, -0.5,      
             0.5,  0.5,  0.5,      
             0.5, -0.5,  0.5,      

             0.5,  0.5, -0.5,      
            -0.5,  0.5, -0.5,      
             0.5,  0.5,  0.5,      

            -0.5,  0.5, -0.5,      
            -0.5,  0.5,  0.5,      
             0.5,  0.5,  0.5,      

            -0.5,  0.5, -0.5,     
            -0.5, -0.5, -0.5,      
            -0.5,  0.5,  0.5,      

            -0.5, -0.5, -0.5,      
            -0.5, -0.5,  0.5,      
            -0.5,  0.5,  0.5,     

            -0.5,  0.5, -0.5,      
             0.5,  0.5, -0.5,      
             0.5, -0.5, -0.5,      

             0.5, -0.5, -0.5,     
            -0.5, -0.5, -0.5,      
            -0.5,  0.5, -0.5,      

            -0.5, -0.5,  0.5,      
             0.5, -0.5,  0.5,      
             0.5,  0.5,  0.5,     

             0.5,  0.5,  0.5,     
            -0.5,  0.5,  0.5,    
            -0.5, -0.5,  0.5,    
	 ]);

	 var vertex_texcoord = new Float32Array
	 ([
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,

			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,

			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,

			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,

			1.0, 0.0,
			0.0, 0.0,
			1.0, 1.0,

			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,

			1.0, 0.0,
            0.0, 0.0,
            1.0, 1.0,

            0.0, 0.0,
            0.0, 1.0,
            1.0, 1.0,

            0.0, 1.0,
            1.0, 1.0,
            1.0, 0.0,

            1.0, 0.0,
            0.0, 0.0,
            0.0, 1.0,

            0.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,

            1.0, 1.0,
            0.0, 1.0,
            0.0, 0.0,
	 ]);

	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

		position_buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, position_buffer);
		gl.bufferData(gl.ARRAY_BUFFER, vertex_cube, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_POSITION,3,gl.FLOAT,false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		texture_buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, texture_buffer);
		gl.bufferData(gl.ARRAY_BUFFER, vertex_texcoord, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		index_buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, index_buffer);
		gl.bufferData(gl.ARRAY_BUFFER, vertex_indices, gl.STATIC_DRAW);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		gl.bindVertexArray(null);

		fbo = gl.createBuffer();
		fbo = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

		color_texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, color_texture);
        gl.texStorage2D(gl.TEXTURE_2D, 9, gl.RGBA8, 512, 512);

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
   	    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    
        depth_texture = gl.createTexture();
	    gl.bindTexture(gl.TEXTURE_2D,depth_texture);
	    gl.texStorage2D(gl.TEXTURE_2D, 9, gl.DEPTH_COMPONENT32F, 512, 512);
        
		gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, color_texture, 0);
		gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D, depth_texture, 0);

		var draw_buffers = [gl.COLOR_ATTACHMENT0];

	gl.drawBuffers(draw_buffers);

	gl.bindFramebuffer(gl.FRAMEBUFFER,null);

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
		canvas.width = canvas_orginal_width;
		canvas.height = canvas_orginal_height;
	}

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);


	var green_color = new Float32Array([0.0, 0.3, 0.0, 1.0]);
	var blue_color  = new Float32Array([0.0, 0.0, 0.3, 1.0]);
	var one 		= new Float32Array([1.0]);

	var projection_matrix = mat4.create();
	var mv_matrix = mat4.create();

	projection_matrix = perspectiveProjectionMatrix;

	mat4.translate(mv_matrix, mv_matrix, [0.0, 0.0, -3.0]);
	mat4.rotateX(mv_matrix, mv_matrix, degToRad(cube_angle));
	mat4.rotateY(mv_matrix, mv_matrix, degToRad(cube_angle));
	mat4.rotateZ(mv_matrix, mv_matrix, degToRad(cube_angle));

	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	gl.viewport(0, 0, 512, 512);
	gl.clearBufferfv(gl.COLOR, 0, green_color);
	gl.clearBufferfi(gl.DEPTH_STENCIL, 0, 1.0, 0);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	gl.useProgram(shaderProgramObject_1);
		gl.uniformMatrix4fv(mvpUniform_1, false, mv_matrix);
		gl.uniformMatrix4fv(projectionUniform_1, false, projection_matrix);

		gl.bindVertexArray(vao);
			gl.drawArrays(gl.TRIANGLES, 0, 36);
		gl.bindVertexArray(null);

		gl.bindFramebuffer(gl.FRAMEBUFFER, null);
	gl.useProgram(null);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	gl.viewport(0, 0, canvas.width, canvas.height);

	gl.clearBufferfv(gl.COLOR, 0, blue_color);
	gl.clearBufferfv(gl.DEPTH, 0, one);

	gl.bindTexture(gl.TEXTURE_2D, color_texture);
	gl.useProgram(shaderProgramObject_2);
		gl.uniformMatrix4fv(mvpUniform_2, false, mv_matrix);
		gl.uniformMatrix4fv(projectionUniform_2, false, projection_matrix);
		gl.bindVertexArray(vao);
			gl.drawArrays(gl.TRIANGLES, 0, 36);
		gl.bindVertexArray(null);
	gl.bindTexture(gl.TEXTURE_2D, null);
	gl.useProgram(null);

	cube_angle = cube_angle + 1.0;
	if(cube_angle >= 360.0)
	{
		cube_angle = 0.0;
	}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	requestAnimationFrame(draw, canvas);

}

function degToRad(angle)
{
	return (angle * Math.PI / 180.0);
}

function uninitialize()
{
	// code
	if(vao)
	{
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if(position_buffer)
	{
		gl.deleteBuffer(position_buffer);
		position_buffer = null;
	}

	if(fbo)
	{
		gl.deleteVertexArray(fbo);
		fbo = null;
	}

	if(shaderProgramObject_1)
	{
		if(fragmentShaderObject_1)
		{
			gl.detachShader(shaderProgramObject_1, fragmentShaderObject_1);
			gl.deleteShader(fragmentShaderObject_1);
			vertexShaderObject = null;
		}

		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject_1, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject_1);
		shaderProgramObject_1 = null;
	}

	if(shaderProgramObject_2)
	{
		if(fragmentShaderObject_2)
		{
			gl.detachShader(shaderProgramObject_2, fragmentShaderObject_2);
			gl.deleteShader(fragmentShaderObject_2);
			vertexShaderObject = null;
		}

		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject_2, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject_2);
		shaderProgramObject_1 = null;
	}
}