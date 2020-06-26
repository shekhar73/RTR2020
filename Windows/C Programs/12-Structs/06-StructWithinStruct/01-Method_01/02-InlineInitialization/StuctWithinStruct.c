#include <stdio.h>

struct Rectangle
{
	struct Rectangle
	{
		int ix;
		int iy;
	} point1, point2;
} rectObj = { {2, 3}, {4, 5} };

int main(void)
{
	//variable declarations
	int iLength, iBreadth, iArea;
	
	//code
	iLength = rectObj.point2.iy - rectObj.point1.y;
	if(iLength < 0)
		iLength = iLength * (-1);
	
	iBreadth = rectObj.point2.ix - rectObj.point1.ix;
	if(iBreadth < 0)
		iBreadth = iBreadth * (-1);
	
	iArea = iLength * iBreadth;
	
	printf("\n\n");
	printf("\tLength of Rectangle = %d\n", iLength);
	printf("\tBreadth of Rectangle = %d\n", iBreadth);
	printf("\tArea of Rectangle = %d\n", iArea);
	
	return (0);
}