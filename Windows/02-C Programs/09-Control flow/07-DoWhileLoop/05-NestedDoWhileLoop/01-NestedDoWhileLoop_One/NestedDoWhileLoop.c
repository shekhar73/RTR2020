#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");
	inum = 1;
	do
	{
		printf("inum = %d\n", inum);
		printf("--------\n\n");

		jnum = 1;
		do
		{
			printf("\tjnum = %d\n", jnum);
			jnum++;
		}while (jnum <= 5);
		
		inum++;
		printf("\n\n");
	}while (inum <= 10);
	
	return(0);
}