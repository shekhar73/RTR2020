#include <stdio.h>
int main(void)
{
	//variable declarations
	int int_ia, int_ib, int_ic;
	
	//code
	int_ia = 9;
	int_ib = 30;
	int_ic = 30;

	// *** FIRST if-else PAIR ***
	printf("\n\n");
	if (int_ia < int_ib)
	{
		printf("Entering First if-block...\n\n");
		printf("A Is Less Than B !!!\n\n");
	}
	else
	{
		printf("Entering First else-block...\n\n");
		printf("A Is NOT Less Than B !!!\n\n");
	}
	printf("First if-else Pair Done !!!\n\n");

	// *** SECOND if-else PAIR ***
	printf("\n\n");
	if (int_ib != int_ic)
	{
		printf("Entering Second if-block...\n\n");
		printf("B Is NOT Equal To P !!!\n\n");
	}
	else
	{
		printf("Entering Second else-block...\n\n");
		printf("B Is Equal To P !!!\n\n");
	}
	printf("Second if-else Pair Done !!!\n\n");
	return(0);
}