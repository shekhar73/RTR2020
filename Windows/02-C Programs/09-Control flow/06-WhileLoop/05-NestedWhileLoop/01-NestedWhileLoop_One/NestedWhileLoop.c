#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");
	inum = 1;
	while(inum <= 10)
	{
		printf("i = %d\n", inum);
		printf("--------\n\n");
		jnum = 1;
		while (jnum <= 5)
		{
			printf("\tjnum = %d\n", jnum);
			jnum++;
		}
		inum++;
		printf("\n\n");
	}
	return(0);
}