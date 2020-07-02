#include <stdio.h>

#define NUMBER_ROWS 5
#define NUMBER_COLUMNS  3

int main(void)
{
	//variable declarations
	int intArray[NUMBER_ROWS][NUMBER_COLUMNS];
	int icnt = 0, jcnt = 0;
	
	int **iPtrArray = NULL;
	
	iPtrArray = (int **)malloc(NUMBER_ROWS * sizeof(int *));
	
	if(iPtrArray == NULL)
	{
		printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows failed !!! Exiting now...\n\n", NUMBER_ROWS);
		exit(0);
	}
	else
	{
		printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows has succeeded !!! Exiting now...\n\n", NUMBER_ROWS);
	}

	
	//Allocating memory to each row
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		iPtrArray[icnt] = (int *)malloc(NUMBER_COLUMNS * sizeof(int));
		if(iPtrArray == NULL)
		{
			printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows failed !!! Exiting now...\n\n", icnt);
			exit(0);
		}
		else
		{
			printf("\tMemory allocation to the 1D arrray of base addresses of %d Rows has succeeded !!! Exiting now...\n\n", icnt);
		}
	}
	
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			*(*(iPtrArray + icnt) + jcnt) = (icnt + 1) * (jcnt + 1);
		}	
	}
	
	
	//Displaying values
	printf("\n\n");
	printf("\t2D integer array elements along with addresses : \n\n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			printf("\tiPtrArray[%d][%d] = %d\t\t at address &iPtrArray[%d][%d]: %p\n", 
			icnt, jcnt, iPtrArray[icnt][jcnt], icnt, jcnt, &iPtrArray[icnt][jcnt]);
		}
		printf("\n\n");
	}
	
	//Freeing allocated memory
	//Freeing memory for each row
	for(icnt = (NUMBER_ROWS - 1); icnt >= 0; icnt--)
	{
		if(*(iPtrArray + icnt))
		{
			free(*(iPtrArray + icnt));
			*(iPtrArray + icnt) = NULL;
			printf("\tMemory allocated to row %d has been successfully freed!!!\n\n", icnt);
		}
	}
	
	if(iPtrArray)
		{
			free(iPtrArray);
			iPtrArray = NULL;
			printf("\tMemory allocated to iPtrArray has been successfully freed!!!\n\n");
		}
	
	return (0);
}