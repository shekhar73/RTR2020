#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum;

	//code
	printf("\n\n");
	printf("Printing Digits from 10 to 1 : \n\n");

	inum = 10;
	while (inum >= 1)
	{
		printf("\t%d\n", inum);
		inum--;
	}
	printf("\n\n");
	return(0);
}