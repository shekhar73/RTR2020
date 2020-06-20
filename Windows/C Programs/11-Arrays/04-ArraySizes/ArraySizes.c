#include <stdio.h>

int main(void)
{
	//variable declarations
	int iArrayOne[5];
	int iArrayTwo[5][3];
	int iArrayThree[120][100][5];
	
	int NumberRows2D;
	int NumberColumns2D;
	
	int NumberRows3D;
	int NumberColumns3D;
	int Depth3D;
	
	//code
	printf("\t\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tSize of 1-D integer array iArrayOne = %lu\n", sizeof(iArrayOne));
	printf("\tNumber of elements in 1-D integer array iArrayOne = %lu\n", (sizeof(iArrayOne)));
	
	printf("\t\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tSize of 2-D integer array iArrayTwo = %lu\n", sizeof(iArrayTwo));
	
	printf("\tNumber of rows in 2-D integer array iArrayTwo = %lu\n", sizeof(iArrayTwo) / sizeof(iArrayTwo[0]));
	NumberRows2D = (sizeof(iArrayTwo) / sizeof(iArrayTwo[0]));
	
	printf("\tNumber of elements (columns) in each row in 2-D integer array iArrayTwo = %lu\n", (sizeof(iArrayTwo[0]) / sizeof(iArrayTwo[0][0])));
	NumberColumns2D = (sizeof(iArrayTwo[0]) / sizeof(iArrayTwo[0][0]));
	
	printf("\tNumber of elements in total in 2D array iArrayTwo = %d\n", (NumberRows2D * NumberColumns2D));
	
	printf("\t\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tSize of 3D integer array iArrayThree = %lu\n", sizeof(iArrayThree));
	
	printf("\tNumber of rows in 3D integer array iArrayThree = %lu\n", sizeof(iArrayThree) / sizeof(iArrayThree[0]));
	NumberColumns3D = (sizeof(iArrayThree[0]));
	NumberRows3D = (sizeof(iArrayThree) / sizeof(iArrayThree[0]));
	
	printf("\tNumber of elements (columns) in one row in 3D integer array iArrayThree = %lu\n", (sizeof(iArrayThree[0]) / sizeof(iArrayThree[0][0])));
	NumberColumns3D = (sizeof(iArrayThree[0]) / sizeof(iArrayThree[0][0]));
	
	printf("\tNumber of elements (depth) in one column in one row in 3D integer array iArrayThree = %lu\n", (sizeof(iArrayThree[0][0]) / sizeof(iArrayThree[0][0][0])));
	Depth3D = (sizeof(iArrayThree[0][0]) / sizeof(iArrayThree[0][0][0]));
	
	printf("\tNumber of elements in total in 3-D Array iArray_Three = %d\n", (NumberRows3D * NumberColumns3D * Depth3D));
	printf("\n\n");
	return (0);
}