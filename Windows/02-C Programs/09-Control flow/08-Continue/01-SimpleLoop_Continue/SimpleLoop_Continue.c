#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum;

	//code
	printf("\n\n");
	printf("Printing Even Numbers From 0 to 100: \n\n");
	for (inum = 0; inum <= 100; inum++)
	{
		// condition for a number to be even number => division of number by 2 leaves no remainder (remainder = 0)
		// if remainder is not 0, the number is odd number...

		if (inum % 2 != 0)
		{
			continue;
		}
		else
		{
			printf("\t%d\n", inum);
		}
	}
	printf("\n\n");
	return(0);
}