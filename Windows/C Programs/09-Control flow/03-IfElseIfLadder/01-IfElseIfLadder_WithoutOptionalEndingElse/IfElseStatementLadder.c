#include <stdio.h>
int main(void)
{
	//variable declarations
	int int_inum;
	//code
	printf("\n\n");
	printf("Enter Value For 'int_inum' : ");
	scanf("%d", &int_inum);

	// IF - ELSE - IF LADDER BEGINS FROM HERE...

	if (int_inum < 0)
	{
		printf("Num = %d Is Less Than 0 (NEGATIVE) !!!\n\n", int_inum);
	}
	else if ((int_inum > 0) && (int_inum <= 100))
	{
		printf("Num = %d Is Between 1 And 100 !!!\n\n", int_inum);
	}
	
	else if ((int_inum > 100) && (int_inum <= 200))
	{
		printf("Num = %d Is Between 101 And 200 !!!\n\n", int_inum);
	}
	
	else if ((int_inum > 200) && (int_inum <= 300))
	{
		printf("Num = %d Is Between 201 And 300 !!!\n\n", int_inum);
	}
	
	else if ((int_inum > 300) && (int_inum <= 400))
	{
		printf("Num = %d Is Between 301 And 400 !!!\n\n", int_inum);
	}
	else if ((int_inum > 400) && (int_inum <= 500))
	{
		printf("Num = %d Is Between 401 And 500 !!!\n\n", int_inum);	
	}
	
	else if (int_inum > 500)
	{
		printf("Num = %d Is Greater Than 500 !!!\n\n", int_inum);	
	}
	
	// *** NO TERMINATING 'ELSE' IN THIS LADDER !!! ***
	return(0);
}