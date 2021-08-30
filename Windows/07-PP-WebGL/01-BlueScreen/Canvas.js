var canvas = document.getElementById('ssk_my_canvas');

var gl = canvas.getContext("webgl2");

var canvas_orginal_width;
var canvas_orginal_height;

var bFullscreen = false;

var requestAnimationFrame = window.requestAnimationFrame		||
							window.webkitRequestAnimationFrame	||
							window.mozRequestAnimationFrame		||
							window.oRequestAnimationFrame		||
							window.msRequestAnimationFrame;

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
	

	window.addEventListener("resize", resize, false);

	init();

	resize();	// warmpu REPAINT call due to below draw()

	draw();

}

function keyDown()
{
	switch(event.keyCode)
	{
		case 70: 	// F
			toggleFullscreen();
			break;
	}
}

function mouseDown()
{
	
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

	gl.clearColor(0.0, 0.0, 1.0, 1.0);
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

}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	requestAnimationFrame(draw, canvas);

}