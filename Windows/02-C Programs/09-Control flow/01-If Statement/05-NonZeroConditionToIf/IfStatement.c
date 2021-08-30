#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum;

	//code
	printf("\n\n");
	
	inum = 5;
	if (inum) // Non-zero Positive Value
	{
		printf("if-block 1 : 'A' Exists And Has Value = %d !!!\n\n", inum);
	}
	
	inum = -5;
	if (inum) // Non-zero Negative Value
	{
		printf("if-block 2 : 'A' Exists And Has Value = %d !!!\n\n", inum);
	}
	
	inum = 0;
	if (inum) // Zero Value
	{
		printf("if-block 3 : 'A' Exists And Has Value = %d !!!\n\n", inum);
	}
	
	printf("All Three if-statements Are Done !!!\n\n");
	return(0);
}