#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");
	printf("Printing Digits 10 to 1 and 100 to 10: \n\n");

	inum = 10;
	jnum = 100;
	while (inum >= 1, jnum >= 10)
	{
		printf("\t %d \t %d\n", inum, jnum);
		inum--;
		jnum = jnum - 10;
	}
	printf("\n\n");
	return(0);
}