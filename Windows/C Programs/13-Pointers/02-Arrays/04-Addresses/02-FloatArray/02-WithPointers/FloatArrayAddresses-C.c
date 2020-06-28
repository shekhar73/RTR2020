#include <stdio.h>

int main(void)
{
	//variable declarations
	float floatArray[10];
	float *ptrArray = NULL;
	int icnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		floatArray[icnt] = (float)(icnt + 1) * 3.5;
	}
	
	ptrArray = floatArray;
	printf("\tElements of the integer array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t floatArray[%d] = %f\n", icnt, *(ptrArray + icnt));
	}
	
	printf("\tElements of the integer array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t floatArray[%d] = %f \t Address = %p\n", icnt, *(ptrArray + icnt), (ptrArray + icnt));
	}
	printf("\n\n");
	return (0);
}