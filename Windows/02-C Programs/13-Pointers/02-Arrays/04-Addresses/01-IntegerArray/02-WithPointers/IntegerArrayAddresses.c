#include <stdio.h>

int main(void)
{
	//variable declarations
	int intArray[10];
	int *ptrArray = NULL;
	int icnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		intArray[icnt] = (icnt + 1) * 10;
	}
	
	ptrArray = intArray;
	printf("\tElements of the integer array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t intArray[%d] = %d\n", icnt, *(ptrArray + icnt));
	}
	
	printf("\tElements of the integer array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t intArray[%d] = %d \t Address = %p\n", icnt, *(ptrArray + icnt), (ptrArray + icnt));
	}
	printf("\n\n");
	return (0);
}