#include <stdio.h>
#define NUMBER_OF_ELEMENTS 10

int main(void)
{
	//variable declarations
	int int_array[NUMBER_OF_ELEMENTS];
	int i, inum, isum = 0;
	
	//code
	printf("\n\n");
	
	// *** ARRAY ELEMENTS INPUT ***
	printf("Enter Integer Elememts For Array : \n\n");
	
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		scanf("%d", &inum);
		int_array[i] = inum;
	}
	
	// *** SEPARATING OUT EVEN NUMBERS FROM ARRAY ELEMENTS ***
	printf("\n\n");
	printf("Even Numbers Amongst The Array Elements Are : \n\n");
	
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		if ((int_array[i] % 2) == 0)
			printf("%d\n", int_array[i]);
	}
	
	// *** SEPARATING OUT ODD NUMBERS FROM ARRAY ELEMENTS ***
	printf("\n\n");
	printf("Odd Numbers Amongst The Array Elements Are : \n\n");
	
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		if ((int_array[i] % 2) != 0)
		printf("%d\n", int_array[i]);
	}
	
	return(0);
}