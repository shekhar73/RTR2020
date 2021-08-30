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
	} rectObj;
	
	//code
	printf("\n\n");
	printf("\tEnter Leftmost X-Coordinate Of Rectangle : ");
	scanf("%d", &rectObj.point1.iX);
	
	printf("\n\n");
	printf("\tEnter Bottommost Y-Coordinate Of Rectangle : ");
	scanf("%d", &rectObj.point1.iY);
	
	printf("\n\n");
	printf("\tEnter Rightmost X-Coordinate Of Rectangle : ");
	scanf("%d", &rectObj.point2.iX);
	
	printf("\n\n");
	printf("\tEnter Topmost Y-Coordinate Of Rectangle : ");
	scanf("%d", &rectObj.point2.iY);
	
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