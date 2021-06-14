function main()
{
	// Step No.1: Get Canvas from DOC
	var canvas = document.getElementById('ssk_my_canvas');
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

	// Step No. 3: Get Drawing Context from the Canvas
	var context = canvas.getContext('2d');
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

	// Step No. 5: Centered the Text
	context.textAlign = "center";		// Horizontally center
	context.Baseline = "middle";		// Vertically center

	// Step No. 6: Set Font
	context.font = "48px sans-serif";

	// Step No. 7: Declare the string to be display
	var str = "Hello World!!!";

	// Step No. 8: Color the text
	context.fillStyle = "white";

	// Step No. 9: color the text
	context.fillText(str, canvas.width / 2, canvas.height / 2);

}