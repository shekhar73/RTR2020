#include<stdio.h>
#include <stdlib.h>

#define NUMBER_ROWS 5
#define NUMBER_COLUMNS_1 3
#define NUMBER_COLUMNS_2 8

int main(void)
{
	//variable declarations
	int *intArray[NUMBER_ROWS];
	
	int icnt = 0, jcnt = 0;
	
	printf("\n\n");
	printf("\t**********************First memory allocation to 2D integer array******************\n\n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		intArray[icnt] = (int *)malloc(NUMBER_COLUMNS_1 * sizeof(int));
		if(intArray[icnt] == NULL)
		{
			printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows failed !!! Exiting now...\n\n", icnt);
			exit(0);
		}
		else
		{
			printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows has succeeded !!! Exiting now...\n\n", icnt);
		}
	}
	
	//assigning values to 2D array
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{

		for(jcnt = 0; jcnt < NUMBER_COLUMNS_1; jcnt++)
		{
			intArray[icnt][jcnt] = (icnt + 1) * (jcnt + 1);
		}
	}
	
	//Displaying values
	printf("\n\n");
	printf("\tDisplaying 2D array : \n\n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS_1; jcnt++)
		{
			printf("\tintArray[%d][%d] = %d\n", icnt, jcnt, intArray[icnt][jcnt]);
		}
		printf("\n\n");
	}
	printf("\n\n");
	
	//Freeing allocated memory
	//Freeing memory for each row
	for(icnt = (NUMBER_ROWS - 1); icnt >= 0; icnt--)
	{
		free(intArray[icnt]);
		intArray[icnt] = NULL;
		printf("\tMemory allocated to row %d has been successfully freed!!!\n\n", icnt);
	}
	
	
	
	
	printf("\n\n");
	printf("\t**********************Second memory allocation to 2D integer array******************\n\n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		intArray[icnt] = (int *)malloc(NUMBER_COLUMNS_2 * sizeof(int));
		if(intArray[icnt] == NULL)
		{
			printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows failed !!! Exiting now...\n\n", icnt);
			exit(0);
		}
		else
		{
			printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows has succeeded !!! Exiting now...\n\n", icnt);
		}
	}
	
	//assigning values to 2D array
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{

		for(jcnt = 0; jcnt < NUMBER_COLUMNS_2; jcnt++)
		{
			intArray[icnt][jcnt] = (icnt + 1) * (jcnt + 1);
		}
	}
	
	//Displaying values
	printf("\n\n");
	printf("\tDisplaying 2D array : \n\n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS_2; jcnt++)
		{
			printf("\tintArray[%d][%d] = %d\n", icnt, jcnt, intArray[icnt][jcnt]);
		}
		printf("\n\n");
	}
	printf("\n\n");
	
	//Freeing allocated memory
	//Freeing memory for each row
	for(icnt = (NUMBER_ROWS - 1); icnt >= 0; icnt--)
	{
		free(intArray[icnt]);
		intArray[icnt] = NULL;
		printf("\tMemory allocated to row %d has been successfully freed!!!\n\n", icnt);
	}
	
	return (0);
}