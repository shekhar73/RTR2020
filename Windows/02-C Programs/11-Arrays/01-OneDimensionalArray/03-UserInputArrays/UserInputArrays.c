#include <stdio.h>

#define INT_ARRAY_NUMBER_OF_ELEMENTS 5
#define FLOAT_ARRAY_NUMBER_OF_ELEMENTS 3
#define CHAR_ARRAY_NUMBER_OF_ELEMENTS 15

int main(void)
{
	
	//variable declarations
	int int_array[INT_ARRAY_NUMBER_OF_ELEMENTS];
	float float_array[FLOAT_ARRAY_NUMBER_OF_ELEMENTS];
	char char_array[CHAR_ARRAY_NUMBER_OF_ELEMENTS];
	int i, number;
	
	//code
	// ********** ARRAY ELEMENTS INPUT **********
	printf("\n\n");
	printf("Enter Elements For 'Integer' Array : \n");
	
	for (i = 0; i < INT_ARRAY_NUMBER_OF_ELEMENTS; i++)
	{
		scanf("%d", &int_array[i]);
	}
	
	printf("\n\n");
	printf("Enter Elements For 'Floating-Point' Array : \n");
	
	for (i = 0; i < FLOAT_ARRAY_NUMBER_OF_ELEMENTS; i++)
	{
		scanf("%f", &float_array[i]);
	}
	
	printf("\n\n");
	printf("Enter Elements For 'Character' Array : \n");
	
	for (i = 0; i < CHAR_ARRAY_NUMBER_OF_ELEMENTS; i++)
	{
		char_array[i] = getch();
		printf("%c\n", char_array[i]);
	}
	
	// ********** ARRAY ELEMENTS OUTPUT **********
	printf("\n\n");
	printf("Integer Array Entered By You : \n\n");
	
	for (i = 0; i < INT_ARRAY_NUMBER_OF_ELEMENTS; i++)
	{
		printf("%d\n", int_array[i]);
	}
	
	printf("\n\n");
	printf("Floating-Point Array Entered By You : \n\n");
	
	for (i = 0; i < FLOAT_ARRAY_NUMBER_OF_ELEMENTS; i++)
	{
		printf("%f\n", float_array[i]);
	}
	
	printf("\n\n");
	printf("Character Array Entered By You : \n\n");
	
	for (i = 0; i < CHAR_ARRAY_NUMBER_OF_ELEMENTS; i++)
	{
		printf("%c\n", char_array[i]);
	}
	
	return(0);
}