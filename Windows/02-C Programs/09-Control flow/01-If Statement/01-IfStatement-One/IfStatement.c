#include <stdio.h>

int main(void)
{
	//variable declarations
	int int_a, int_b, int_c;

	//code
	int_a = 5, int_b = 7, int_c = 22;
	printf("\n\n");

	if(int_a < int_b)
	{
		printf("A = %d is less than B = %d\n\n", int_a, int_b);
	}		
	if(int_b != int_c)
	{
		printf("B = %d is not Equal to C = %d\n\n", int_b, int_c);
	}
	printf("Both Comparisons have been done !!!\n");
	return (0);
}
