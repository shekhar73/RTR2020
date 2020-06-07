#include <stdio.h>
#define NUMBER_OF_ELEMENTS 10

int main(void)
{
	//variable declarations
	int int_array[NUMBER_OF_ELEMENTS];
	int i, number, sum = 0;
	
	//code
	printf("\n\n");
	printf("Enter Integer Elememts For Array : \n\n");
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		scanf("%d", &int_array[i]);
	}
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		sum = sum + int_array[i];
	}
	printf("\n\n");
	printf("SUM Of all elements of array = %d\n\n", sum);
	return(0);
}