#include <stdio.h>
int main(void)
{
	//variable declarations
	int iLength, iBreadth, iArea;
	
	struct Rectangle
	{
		struct MyPoint
		{
			int iX;
			int iY;
		} point1, point2;
	} rectObj = { {2, 5}, {7, 8} };
	
	//code
	
	iLength = rectObj.point2.iY - rectObj.point1.iY;
	if (iLength < 0)
		iLength = iLength * -1;
	
	iBreadth = rectObj.point2.iX - rectObj.point1.iX;
	if (iBreadth < 0)
		iBreadth = iBreadth * -1;
	
	iArea = iLength * iBreadth;
	
	printf("\n\n");
	printf("\tiLength Of Rectangle = %d\n\n", iLength);
	printf("\tiBreadth Of Rectangle = %d\n\n", iBreadth);
	printf("\tArea Of Rectangle = %d\n\n", iArea);
	return(0);
}