#include <stdio.h>

struct Rectangle
{
	struct MyPoint
	{
		int ix;
		int iy;
	} point1, point2;
} objRect;

int main(void)
{
	//variable declarations
	int iLength, iBreadth, iArea;
	
	//code
	printf("\n\n");
	printf("\tEnter leftmost X-coordinate of Rectangle : ");
	scanf("%d", &objRect.point1.ix);
	
	printf("\n\n");
	printf("\tEnter Bottommost Y-coordinate of Rectangle : ");
	scanf("%d", &objRect.point1.iy);
	
	printf("\n\n");
	printf("\tEnter Rigthmost X-coordinate of Rectangle : ");
	scanf("%d", &objRect.point2.ix);
	
	printf("\n\n");
	printf("\tEnter bottommost Y-coordinate of Rectangle : ");
	scanf("%d", &objRect.point2.iy);
	
	iLength = objRect.point2.iy - objRect.point1.iy;
	if(iLength < 0)
		iLength = iLength * (-1);
	
	iBreadth = objRect.point2.ix - objRect.point1.ix;
	if(iBreadth < 0)
		iBreadth = iBreadth * (-1);
	
	iArea = iLength * iBreadth;
	
	printf("\n\n");
	printf("\tLength of Rectangle = %d\n", iLength);
	printf("\tBreadth of Rectangle = %d\n", iBreadth);
	printf("\tArea of Rectangle = %d\n", iArea);
	
	return (0);
}