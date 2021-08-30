#include <stdio.h>
#include <stdlib.h>

#define NUMBER_ROWS 5
#define NUMBER_COLUMNS  3

int main(void)
{
	//variable declarations
	int intArray[NUMBER_ROWS][NUMBER_COLUMNS];
	int icnt = 0, jcnt = 0;
	
	//code
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			*(intArray[icnt] + jcnt) = (icnt + 1) * (jcnt + 1);
		}
	}
	
	printf("\n\n");
	printf("\t2D integer array elements along with addresses : \n\n");
	for(icnt = 0; icnt < NUMBER_ROWS; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_COLUMNS; jcnt++)
		{
			printf("\t*(intArray[%d] + %d) = %d\t\t at address : %p\n", 
			icnt, jcnt, *(intArray[icnt] + jcnt), (intArray[icnt] + jcnt));
		}
		printf("\n\n");
	}
	
	return (0);
}