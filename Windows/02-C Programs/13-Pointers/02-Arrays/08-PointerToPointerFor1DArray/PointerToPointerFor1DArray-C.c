#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	void MyAlloc(int **iPtr, unsigned int numberOfElements);
	
	//variable declarations
	int *piArray = NULL;
	unsigned int numberElements;
	int icnt;
	
	//code
	printf("\n\n");
	printf("\tHow many elements you want in integer array?");
	scanf("%u", &numberElements);
	
	printf("\n\n");
	MyAlloc(&piArray, numberElements);
	
	printf("\tEnter %u elements to fill up your integer array :\n", numberElements);
	for(icnt = 0; icnt < numberElements; icnt++)
	{
		scanf("\t %d",&piArray[icnt]);
	}
	printf("\n\n");
	printf("\tThe %u  elements entered by you in the integer array : \n\n", numberElements);
	for(icnt = 0; icnt < numberElements; icnt++)
	{
		printf("\t %u\n", piArray[icnt]);
	}
	
	printf("\n\n");
	if(piArray)
	{
		free(piArray);
		piArray = NULL;
	}
	
	return (0);
}

void MyAlloc(int **ptr, unsigned int numberOfElements)
{
	//code
	*ptr = (int *)malloc(numberOfElements * sizeof(int));
	if(*ptr == NULL)
	{
		printf("\tCould not allocate memory!!! Exiting now...\n\n");
		exit(0);
	}
	printf("\tMyAlloc() has been allocated successfully %lu bytes for integer array!!!\n\n", (numberOfElements * sizeof(int)));
	
	
}