#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");
	printf("Printing Digits from 1 to 10 and 10 to 100: \n\n");
	
	inum = 1;
	jnum = 10;

	while (inum <= 10, jnum <= 100)
	{
		printf("\t %d \t %d\n", inum, jnum);
		inum++;
		jnum = jnum + 10;
	}
	printf("\n\n");
	return(0);
}