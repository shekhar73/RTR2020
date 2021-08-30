#include <stdio.h>

int main(void)
{
	//variable declarations
	double doubleArray[10];
	int icnt;
	double *ptrArray = NULL;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		doubleArray[icnt] = (float)(icnt + 1) * 2.343453f;
	}
	
	ptrArray = doubleArray;
	
	printf("\n\n");
	printf("\tElements of the double array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t doubleArray[%d] = %lf\n", icnt, *(ptrArray + icnt));
	}
	
	printf("\n\n");
	printf("\tElements of the double array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t doubleArray[%d] = %lf\t\t address = %p\n", icnt, *(ptrArray + icnt), (ptrArray + icnt));
	}
	
	printf("\n\n");
	
	return (0);
}