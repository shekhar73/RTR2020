#include <stdio.h>

//Defining struct

struct MyPoint
{
	int x;
	int y;
} point; //declaraing a single variable of type 'struct MyPoint' globally...

//Defining struct

struct MyPointProperties
{
	int iQuadrant;
	char cAxisLocation[10];
} point_properties; //declaraing a single variable of type 'struct MyPointProperties' globally...

int main(void)
{
	//code
	//User input for the data members of struct MyPoint vaiable
	printf("\n\n");
	printf("\tEnter X-Coordinates for a point : ");
	scanf("%d", &point.x);
	
	printf("\tEnter y-Coordinates for a point : ");
	scanf("%d", &point.y);
	
	printf("\n\n");
	printf("\tPoint Co-Ordinates (X, Y) are : (%d, %d) !\n\n", point.x, point.y);
	
	if(point.x == 0 && point.y == 0)
		printf("\tThe point is at the Origin (%d, %d) !\n", point.x, point.y);
	else // Atleast One of the two values (either 'X' or 'Y' or BOTH) is a nonzero value...
	{
		if(point.x == 0) //If x = 0 then y is nonzero
		{
			if(point.y < 0) //If y is -ve
				strcpy(point_properties.cAxisLocation, "Y is Negative");
				
			if (point.y > 0) //Y is +ve
				strcpy(point_properties.cAxisLocation, "Y is Positive");
				
			point_properties.iQuadrant = 0; // A Point Lying On Any Of The Coordinate Axes Is NOT A Part Of ANY Quadrant...

			printf("\tThe point lies on the %s axis !\n", point_properties.cAxisLocation);
		}
		else if(point.y == 0) //If Y is zero then X is nonzero
		{
			if(point.x < 0)
				strcpy(point_properties.cAxisLocation, "X is Negative");
			
			if(point.x > 0)
				strcpy(point_properties.cAxisLocation, "X is Positive");
			
			point_properties.iQuadrant = 0;
			printf("\tThe point lies on the %s axis !\n\n", point_properties.cAxisLocation);			
		}
		else //Both  X and Y are Non-Zero
		{
			point_properties.cAxisLocation[0] = '\0';
			
			if(point.x > 0 && point.y > 0) // X is +ve and Y is +pve
				point_properties.iQuadrant = 1;
			
			else if (point.x < 0 && point.y > 0) //X is -ve and Y is +ve
				point_properties.iQuadrant = 2;
			
			else if (point.x < 0 && point.y < 0) //X is -ve and Y is -ve
				point_properties.iQuadrant = 3;
			
			else
				point_properties.iQuadrant = 4; //X is +ve and Y is -ve
			
			printf("\tThe point lies in Quadrant Number %d !!!\n\n", point_properties.iQuadrant);
		}
	}
	
	
	return (0);
}