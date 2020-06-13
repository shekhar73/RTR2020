#include <stdio.h>

// DEFINING STRUCT ...

struct MyPoint
{
	int x;
	int y;
}; 

int main(void)
{
	
	struct MyPoint point_P, point_Q, point_R, point_S, point_T;

	//code
	//Assigning Data Values To The Data Members Of 'struct MyPoint' variable 'point_P'
	point_P.x = 13;
	point_P.y = 7;
	
	//Assigning Data Values To The Data Members Of 'struct MyPoint' variable 'point_Q'
	point_Q.x = 34;
	point_Q.y = 43;
	
	//Assigning Data Values To The Data Members Of 'struct MyPoint' variable 'point_R'
	point_R.x = 4;
	point_R.y = 7;
	
	//Assigning Data Values To The Data Members Of 'struct MyPoint' variable 'point_S'
	point_S.x = 5;
	point_S.y = 7;
	
	//Assigning Data Values To The Data Members Of 'struct MyPoint' variable 'point_T'
	point_T.x = 31;
	point_T.y = 82;
	
	//Displaying Values Of The Data Members Of 'struct MyPoint' (all variables)
	printf("\n\n");
	printf("\tCo-ordinates (X, Y) Of Point 'P' Are : (%d, %d)\n\n", point_P.x, point_P.y);
	printf("\tCo-ordinates (X, Y) Of Point 'Q' Are : (%d, %d)\n\n", point_Q.x, point_Q.y);
	printf("\tCo-ordinates (X, Y) Of Point 'R' Are : (%d, %d)\n\n", point_R.x, point_R.y);
	printf("\tCo-ordinates (X, Y) Of Point 'S' Are : (%d, %d)\n\n", point_S.x, point_S.y);
	printf("\tCo-ordinates (X, Y) Of Point 'T' Are : (%d, %d)\n\n", point_T.x, point_T.y);
	
	return(0);
}