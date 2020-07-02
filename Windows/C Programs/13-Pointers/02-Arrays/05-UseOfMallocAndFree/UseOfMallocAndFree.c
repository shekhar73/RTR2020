#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	//variable declarations
	int *ptrArray = NULL;
	unsigned int intArrayLength = 0;
	int icnt = 0;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter the number of elements you want in your integer array : ");
	scanf("%d", &intArrayLength);
	
	ptrArray = (int *)malloc(sizeof(int) * intArrayLength);
	
	if(ptrArray == NULL)
	{
		printf("\n\n");
		printf("\tMemory allocation for integer array has failed !!! Exiting now...\n\n");
		exit (0);
	}
	else
	{
		printf("\n\n");
		printf("\tMemory allocation for integer array has succeeded !!!\n\n");
		printf("\tMemoory addresses from %p to %p have been allocated to integer array !!!\n\n", 
		ptrArray, (ptrArray + (intArrayLength - 1)));
	}
	
	printf("\n\n");
	printf("\tEnter %d elements for the integer array : \n\n", intArrayLength);
	for(icnt = 0; icnt < intArrayLength; icnt++)
	{
		scanf("\t%d", (ptrArray + icnt));
	}
	
	printf("\n\n");
	printf("\tThe integer array entered by you, consisting of %d elements : \n\n", intArrayLength);
	for(icnt = 0; icnt < intArrayLength; icnt++)
	{
		printf("\tptrArray[%d] = %d\t\t at address &ptrArray[%d] = %p\n",
		icnt, *(ptrArray + icnt), icnt, (ptrArray + icnt));
	}
	
	if(ptrArray)
	{
		free(ptrArray);
		ptrArray = NULL;
		
		printf("\n\n");
		printf("\tMemoory allocated for integer array has been successfully freed !!!\n");
	}
	
	return (0);
}