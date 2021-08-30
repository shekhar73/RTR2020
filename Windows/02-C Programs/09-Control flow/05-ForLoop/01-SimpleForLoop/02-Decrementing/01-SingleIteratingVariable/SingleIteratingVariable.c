#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum;

	//code
	printf("\n\n");
	printf("Printing the digits from 1 to 10 : \n\n");
	for (inum = 10; inum >= 1; inum--)
	{
		printf("\t%d\n", inum);
	}
	printf("\n\n");
	return(0);
}