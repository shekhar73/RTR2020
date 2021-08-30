#include<stdio.h>

#define NUMBER_ROWS 5
#define NUMBER_COLUMNS 3

int main(void)
{
	//variable declarations
	int iArray_2D[NUMBER_ROWS][NUMBER_COLUMNS];
	int iArray_1D[NUMBER_ROWS * NUMBER_COLUMNS];
	
	int icnt, jcnt;
	int iNum;
	
	//code
	printf("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter elements of your choice to fill up the integer 2D array : \n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		printf("\tFor ROW_NUMER %D : \n", (icnt + 1));
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			printf("\tEnter element number %d : ", (jcnt + 1));
			scanf("%d", &iNum);
			iArray_2D[icnt][jcnt] = iNum;
		}
		printf("\n\n");
	}
	
	//Display of 2D array
	printf("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tTwo-Dimensional (2D) array of integers : \n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		printf("\t----------------------------------ROW %d--------------------------------\n", (icnt + 1));
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			printf("\tiArray_2D[%d][%d] = %d\t", icnt, jcnt, iArray_2D[icnt][jcnt]);
		}
		printf("\n");
	}
	
	//Converting 2D integer array to 1D integer array
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			iArray_1D[(icnt * NUMBER_COLUMNS) + jcnt] = iArray_2D[icnt][jcnt];
		}
	}
	
	//printing 1D array
	printf("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("One-Dimensional (1D) array of integers : \n\n");
	for(icnt = 0; icnt < (NUMBER_ROWS * NUMBER_COLUMNS); icnt++)
	{
		printf("\tiArray_1D[%d] = %d", icnt, iArray_1D[icnt]);
	}
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	return (0);
}