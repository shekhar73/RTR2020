// Step No.1: Get Canvas from DOC
var canvas = document.getElementById('ssk_my_canvas');


// Step No. 3: Get Drawing Context from the Canvas
var context = canvas.getContext('2d');

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

	// Step No. 2: Retrive the width and height of canvas for sake of information
	console.log("Canvas width = "+canvas.width+" Canvas height = "+canvas.height+"\n");

	
	if(!context)
	{
		console.log("Obtaining Context failed\n");
	}
	else
	{
		console.log("Obtaining Context Succeded\n");
	}

	// Step No. 4: Paint the background by Black Color
	context.fillStyle = 'black';		// Internally like setter function, its called as properties
	context.fillRect(0, 0, canvas.width, canvas.height);	// fillRect(x, y, width, height);

	drawText("Hello World!!!");

	
	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	
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

function drawText(text)		// in js based browser there is no REPAINT so we have added it manually by using drawText()
{
	// Step No. 5: Centered the Text
	context.textAlign = "center";		// Horizontally center
	context.Baseline = "middle";		// Vertically center

	// Step No. 6: Set Font
	context.font = "48px sans-serif";

	// Step No. 7: Color the text
	context.fillStyle = "white";

	// Step No. 8: color the text
	context.fillText(text, canvas.width / 2, canvas.height / 2);
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
	}
}
