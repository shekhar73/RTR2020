#include <stdio.h>

int main(void)
{
	//variable declarations
	//In-line initialization
	
	int iArray[5][3][2] = { { {9, 4}, {5, 4}, {5, 2} },
							{ {8, 5}, {3, 2}, {1, 2} },
							{ {6, 2}, {8, 9}, {7, 7} },
							{ {9, 3}, {2, 3}, {5, 5} },
							{ {6, 9}, {5, 2}, {3, 3} },
						  };
						  
	int intSize;
	int iArraySize;
	int iArrayNumElements, iArrayWidth, iArrayHeight, iArrayDepth;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	intSize = sizeof(int);
		
	iArraySize = sizeof(iArray);
	printf("\tSize of three dimensional (3D) integer array is = %d\n\n", iArraySize);
	
	iArrayWidth = iArraySize / sizeof(iArray[0]);
	printf("\tNumber of Rows (width) in three-dimensional (3D) integer array is = %d\n\n", iArrayWidth);
	
	iArrayHeight = sizeof(iArray[0]) / sizeof(iArray[0][0]);
	printf("\tNumber of columns (Height) in three dimensional (3D) integer array is = %d\n\n", iArrayHeight);
	
	iArrayDepth = sizeof(iArray[0][0]) / intSize;
	printf("\tDepth of three dimensional (3D) integer array is = %d\n\n", iArrayDepth);
	
	iArrayNumElements = iArrayWidth * iArrayHeight * iArrayDepth;
	printf("\tNumber of elements in three dimensional (3D) integer array is = %d\n\n", iArrayNumElements);

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tElements in integer 3D array : \n\n");
	
	//Piece meal dipslay
	//Row1
	printf("\t****** ROW 1 ******\n");
	printf("\t****** COLUMN 1 ******\n");
	
	printf("\tiArray[0][0][0] = %d\n", iArray[0][0][0]);
	printf("\tiArray[0][0][1] = %d\n", iArray[0][0][1]);
	
	printf("\n");
	printf("\t****** COLUMN 2 ******\n");
	printf("\tiArray[0][1][0] = %d\n", iArray[0][1][0]);
	printf("\tiArray[0][1][1] = %d\n", iArray[0][1][1]);
	
	printf("\t\n");
	printf("\t****** COLUMN 3 ******\n");
	printf("\tiArray[0][2][0] = %d\n", iArray[0][2][0]);
	printf("\tiArray[0][2][1] = %d\n", iArray[0][2][1]);
	
	printf("\n\n");
	// ****** ROW 2 ******
	printf("\t****** ROW 2 ******\n");
	printf("\t****** COLUMN 1 ******\n");
	
	printf("\tiArray[1][0][0] = %d\n", iArray[1][0][0]);
	printf("\tiArray[1][0][1] = %d\n", iArray[1][0][1]);
	
	printf("\n");
	printf("\t****** COLUMN 2 ******\n");
	printf("\tiArray[1][1][0] = %d\n", iArray[1][1][0]);
	printf("\tiArray[1][1][1] = %d\n", iArray[1][1][1]);
	
	printf("\n");
	printf("\t****** COLUMN 3 ******\n");
	printf("\tiArray[1][2][0] = %d\n", iArray[1][2][0]);
	printf("\tiArray[1][2][1] = %d\n", iArray[1][2][1]);
	printf("\n\n");
	
	// ****** ROW 3 ******
	printf("\t****** ROW 3 ******\n");
	printf("\t****** COLUMN 1 ******\n");
	printf("\tiArray[2][0][0] = %d\n", iArray[2][0][0]);
	printf("\tiArray[2][0][1] = %d\n", iArray[2][0][1]);
	
	printf("\n");
	printf("\t****** COLUMN 2 ******\n");
	printf("\tiArray[2][1][0] = %d\n", iArray[2][1][0]);
	printf("\tiArray[2][1][1] = %d\n", iArray[2][1][1]);
	
	printf("\n");
	printf("\t****** COLUMN 3 ******\n");
	printf("\tiArray[2][2][0] = %d\n", iArray[2][2][0]);
	printf("\tiArray[2][2][1] = %d\n", iArray[2][2][1]);
	printf("\n\n");
	
	// ****** ROW 4 ******
	printf("\t****** ROW 4 ******\n");
	printf("\t****** COLUMN 1 ******\n");
	printf("\tiArray[3][0][0] = %d\n", iArray[3][0][0]);
	printf("\tiArray[3][0][1] = %d\n", iArray[3][0][1]);
	
	printf("\n");
	printf("\t****** COLUMN 2 ******\n");
	printf("\tiArray[3][1][0] = %d\n", iArray[3][1][0]);
	printf("\tiArray[3][1][1] = %d\n", iArray[3][1][1]);
	
	printf("\n");
	printf("\t****** COLUMN 3 ******\n");
	printf("\tiArray[3][2][0] = %d\n", iArray[3][2][0]);
	printf("\tiArray[3][2][1] = %d\n", iArray[3][2][1]);
	printf("\n\n");
	
	// ****** ROW 5 ******
	printf("\t****** ROW 5 ******\n");
	printf("\t****** COLUMN 1 ******\n");
	printf("\tiArray[4][0][0] = %d\n", iArray[4][0][0]);
	printf("\tiArray[4][0][1] = %d\n", iArray[4][0][1]);
	
	printf("\n");
	printf("\t****** COLUMN 2 ******\n");
	printf("\tiArray[4][1][0] = %d\n", iArray[4][1][0]);
	printf("\tiArray[4][1][1] = %d\n", iArray[4][1][1]);
	
	printf("\n");
	printf("\t****** COLUMN 3 ******\n");
	printf("\tiArray[4][2][0] = %d\n", iArray[4][2][0]);
	printf("\tiArray[4][2][1] = %d\n", iArray[4][2][1]);
	printf("\n\n");

	return(0);
}