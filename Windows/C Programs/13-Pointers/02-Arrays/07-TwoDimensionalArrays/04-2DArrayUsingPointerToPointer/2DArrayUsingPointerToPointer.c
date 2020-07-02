#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	//variable declarations
	int **iPtrArray = NULL;
	int icnt, jcnt;
	int numberRows, numberColumns;
	
	//code
	
	//**********Accept Number of Rows numberRows from User*******
	printf("\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\n\tEnter number of rows : ");
	scanf("%d", &numberRows);
	
	//**********Accept number of Columns numberColumns from User*******
	printf("\n\n");
	printf("\tEnter number of columns : ");
	scanf("%d", &numberColumns);
	
	
	//*********Allocationg memory to 1D array consisting of base address of rows********
	printf("\n\n");
	printf("\t*******************Memory allocation to 2D integer array******************\n\n");
	
	iPtrArray = (int **)malloc(numberRows * sizeof(int *));
	
	if(iPtrArray == NULL)
	{
		printf("\tMemory allocation faild to %d rows of 2D integer array !!! Exiting now...\n", numberRows);
		exit(0);
	}
	else
	{
		printf("\tMemory allocation to %d Rows has succeeded !!! Exiting now...\n", numberRows);
	}
	
	
	
	for(icnt = 0; icnt < numberRows; icnt++)
	{
		iPtrArray[icnt] = (int *)malloc(numberColumns * sizeof(int));
		
		if(iPtrArray == NULL)
		{
			printf("\tMemory allocation faild to columns of row %d of 2D integer array !!! Exiting now...\n", icnt);
			exit(0);
		}
		else
		{
			printf("\tMemory allocation to columns of row %d of 2D integer array has succeeded !!! Exiting now...\n", icnt);
		}
	}
	
	//*******Filling up values*******
	
	for(icnt = 0; icnt < numberRows; icnt++)
	{
		for(jcnt = 0; jcnt < numberColumns; jcnt++)
		{
			iPtrArray[icnt][jcnt] = (icnt * 1) + (jcnt * 1);
		}
	}
	
	//Displaying values
	for(icnt = 0; icnt < numberRows; icnt++)
	{
		printf("\tBase address of row %d : iPtrArray[%d] = %p \t at address : %p\n", icnt, jcnt, iPtrArray[icnt], &iPtrArray[icnt]);
	}
	
	printf("\n\n");
	for(icnt = 0; icnt < numberRows; icnt++)
	{
		for(jcnt = 0; jcnt < numberColumns; jcnt++)
		{
			printf("\t iPtrArray[%d][%d] = %d \t at address : %p\n", icnt, jcnt, iPtrArray[icnt][jcnt],
			&iPtrArray[icnt][jcnt]);
		}
		printf("\n");
	}
	
	//Freeing memory allocated to each row
	for(icnt = (numberRows - 1); icnt >= 0; icnt--)
	{
		if(iPtrArray[icnt])
		{
			free(iPtrArray[icnt]);
			iPtrArray[icnt] = NULL;
			printf("\tMemory allocated to row %d has been successfully free!!!\n", icnt);
		}
	}
	
	//Freeing memory allocated to 1D array consisting of base address of rows
	if(iPtrArray)
	{
		free(iPtrArray);
		iPtrArray = NULL;
		printf("\tMemory allocated to iPtrArray has been successfully freed!!!\n");
	}
	
	return (0);
}