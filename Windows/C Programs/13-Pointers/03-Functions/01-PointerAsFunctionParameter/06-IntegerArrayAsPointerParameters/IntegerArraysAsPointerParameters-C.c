#include <stdio.h>
#include <stdlib.h>


void MultiplyArrayElementsByNumber(int *arr, int numberOfElements, int Number)
{
	int icnt;
	for(icnt = 0; icnt < numberOfElements; icnt++)
	{
		arr[icnt] = arr[icnt] * Number;
	}
}

int main(void)
{
	//function declarations
	//void MultiplyArrayElementsByNumber(int *, int, int);
	
	//variable declarations
	int *intArray = NULL;
	int numberOfElements;
	int icnt, iNumber;
	
	//code
	printf("\n\n");
	printf("\tEnter How many number of elements you want in the integer array : ");
	scanf("%d", &numberOfElements);
	
	intArray = (int *)malloc(numberOfElements * sizeof(int));
	if(intArray == NULL)
	{
		printf("\tMemory allocation to intArray array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	printf("\n\n");
	printf("\tEnter %d elements for the integer array : ", numberOfElements);
	for(icnt = 0; icnt < numberOfElements; icnt++)
	{
		scanf("%d", &intArray[icnt]);
	}
	
	printf("\n\n");
	printf("\tArray before passing to function MultiplyArrayElementsByNuber() : \n\n");
	for(icnt = 0; icnt < numberOfElements; icnt++)
	{
		printf("\t intArray[%d] = %d\n", icnt, intArray[icnt]);
	}
	
	printf("\n\n");
	printf("\tEnter the number by which you want to multiply each array element : ");
	scanf("%d", &iNumber);
	
	MultiplyArrayElementsByNumber(intArray, numberOfElements, iNumber);
	
	printf("\n\n");
	printf("\tArray returned by function MultiplyArrayElementsByNuber() : \n\n");
	for(icnt = 0; icnt < numberOfElements; icnt++)
	{
		printf("\t intArray[%d] = %d\n", icnt, intArray[icnt]);
	}
	
	if(intArray)
	{
		free(intArray);
		intArray = NULL;
		printf("\n\n");
	}
	return (0);
}
