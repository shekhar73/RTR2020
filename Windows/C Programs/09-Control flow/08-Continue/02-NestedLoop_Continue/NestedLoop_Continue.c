#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");

	printf("Outer Loop prints odd numbers between 1 and 10. \n\n");
	printf("Inner loop prints even numbers between 1 and 10 for Every Odd Number printed by Outer Loop. \n\n");
	
	// condition for a number to be even number => division of number by 2 leaves no remainder (remainder = 0)
	// condition for a number to be odd number => division of number by 2 leaves remainder (remainder = 1 (usually))
	
	for (inum = 1; inum <= 10; inum++)
	{
		if (inum % 2 != 0) //If Number (i) Is Odd..
		{
			printf("inum = %d\n", inum);
			printf("---------\n");

			for (jnum = 1; jnum <= 10; jnum++)
			{
				if (jnum % 2 == 0) //If Number (j) Is Even...
				{
					printf("\tjnum = %d\n",jnum);
				}
				else //If Number (j) Is Odd..
				{
					continue;
				}
			}
			printf("\n\n");
		}
		else //If Number (i) Is Even...
		{
			continue;
		}
	}
	printf("\n\n");
	return(0);
}