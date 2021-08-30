#include <stdio.h>

int main(void)
{
	//variable declarations
	float floatArray[10];
	int icnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		floatArray[icnt] = (float)(icnt + 1) * 1.5f;
	}
	
	printf("\n\n");
	printf("\tElements of the float array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t floatArray[%d] = %f\n", icnt, floatArray[icnt]);
	}
	
	printf("\n\n");
	printf("\tElements of the float array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t floatArray[%d] = %f\t\t address = %p\n", icnt, floatArray[icnt], &floatArray[icnt]);
	}
	
	printf("\n\n");
	
	return (0);
}