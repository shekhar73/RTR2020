#include <stdio.h>

int main(void)
{
	//variable declarations
	double doubleArray[10];
	int icnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		doubleArray[icnt] = (double)(icnt + 1) * 2.343453f;
	}
	
	printf("\n\n");
	printf("\tElements of the double array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t doubleArray[%d] = %lf\n", icnt, doubleArray[icnt]);
	}
	
	printf("\n\n");
	printf("\tElements of the double array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t doubleArray[%d] = %lf\t\t address = %p\n", icnt, doubleArray[icnt], &doubleArray[icnt]);
	}
	
	printf("\n\n");
	
	return (0);
}