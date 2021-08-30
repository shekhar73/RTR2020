#include<stdio.h>

//Defining struct

struct MyPoint
{
	int x;
	int y;
} point_P, point_Q, point_R, point_S, point_T; //Declaring 5 struct variables of type 'struct MyPoint' globally...

int main(void)
{
	//code
	//Assigning data values to the data members of struct MyPoint variables point_P
	point_P.x = 5;
	point_P.y = 7;
	
	//Assigning data values to the data members of struct MyPoint variables point_Q
	point_Q.x = 3;
	point_Q.y = 9;
	
	//Assigning data values to the data members of struct MyPoint variables point_R
	point_R.x = 1;
	point_R.y = 2;
	
	//Assigning data values to the data members of struct MyPoint variables point_S
	point_S.x = 81;
	point_S.y = 10;
	
	//Assigning data values to the data members of struct MyPoint variables point_T
	point_T.x = 23;
	point_T.y = 45;
	
	//Displaying data values to the data members of struct MyPoint (all varibalbes)
	
	printf("\n\n");
	printf("\tCo-ordinates (x, y) of point 'P' are : (%d, %d)\n", point_P.x, point_P.y);
	printf("\tCo-ordinates (x, y) of point 'Q' are : (%d, %d)\n", point_Q.x, point_Q.y);
	printf("\tCo-ordinates (x, y) of point 'R' are : (%d, %d)\n", point_R.x, point_R.y);
	printf("\tCo-ordinates (x, y) of point 'S' are : (%d, %d)\n", point_S.x, point_S.y);
	printf("\tCo-ordinates (x, y) of point 'T' are : (%d, %d)\n", point_T.x, point_T.y);
	
	return (0);
}