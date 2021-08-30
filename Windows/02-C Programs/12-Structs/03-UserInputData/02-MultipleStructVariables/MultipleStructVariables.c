#include <stdio.h>
struct MyPoint
{
	int x;
	int y;
};

int main(void)
{
	//variable declarations
	struct MyPoint point_P, point_Q, point_R, point_S, point_T; //declaring a 5 struct variables
	
	//code
	//User Input For The Data Members Of 'struct MyPoint' variable 'point_P'
	
	printf("\n\n");
	printf("Enter X-Coordinate For Point 'A' : ");
	scanf("%d", &point_P.x);
	
	printf("Enter Y-Coordinate For Point 'A' : ");
	scanf("%d", &point_P.y);
	
	//User Input For The Data Members Of 'struct MyPoint' variable 'point_Q'
	printf("\n\n");
	printf("Enter X-Coordinate For Point 'B' : ");
	scanf("%d", &point_Q.x);
	
	printf("Enter Y-Coordinate For Point 'B' : ");
	scanf("%d", &point_Q.y);
	
	//User Input For The Data Members Of 'struct MyPoint' variable 'point_R'
	printf("\n\n");
	printf("Enter X-Coordinate For Point 'C' : ");
	scanf("%d", &point_R.x);
	
	printf("Enter Y-Coordinate For Point 'C' : ");
	scanf("%d", &point_R.y);
	
	//User Input For The Data Members Of 'struct MyPoint' variable 'point_S'
	printf("\n\n");
	printf("Enter X-Coordinate For Point 'D' : ");
	scanf("%d", &point_S.x);
	
	printf("Enter Y-Coordinate For Point 'D' : ");
	scanf("%d", &point_S.y);
	
	//User Input For The Data Members Of 'struct MyPoint' variable 'point_T'
	printf("\n\n");
	printf("Enter X-Coordinate For Point 'E' : ");
	scanf("%d", &point_T.x);
	
	printf("Enter Y-Coordinate For Point 'E' : ");
	scanf("%d", &point_T.y);
	
	//Displaying Values Of The Data Members Of 'struct MyPoint' (all variables)
	printf("\n\n");
	printf("Co-ordinates (x, y) Of Point 'A' Are : (%d, %d)\n\n", point_P.x, point_P.y);
	printf("Co-ordinates (x, y) Of Point 'B' Are : (%d, %d)\n\n", point_Q.x, point_Q.y);
	printf("Co-ordinates (x, y) Of Point 'C' Are : (%d, %d)\n\n", point_R.x, point_R.y);
	printf("Co-ordinates (x, y) Of Point 'D' Are : (%d, %d)\n\n", point_S.x, point_S.y);
	printf("Co-ordinates (x, y) Of Point 'E' Are : (%d, %d)\n\n", point_T.x, point_T.y);
	return(0);
}