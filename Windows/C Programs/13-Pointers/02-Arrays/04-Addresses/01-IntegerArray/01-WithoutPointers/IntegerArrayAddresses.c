#include <stdio.h>

int main(void)
{
	//variable declarations
	int intArray[10];
	int icnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		intArray[icnt] = (icnt + 1) * 3;
	}
	
	printf("\n\n");
	printf("\tElements of the integer array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t intArray[%d] = %d\n", icnt, intArray[icnt]);
	}
	
	printf("\n\n");
	printf("\tElements of the integer array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t intArray[%d] = %d\t\t address = %p\n", icnt, intArray[icnt], &intArray[icnt]);
	}
	
	printf("\n\n");
	
	return (0);
}