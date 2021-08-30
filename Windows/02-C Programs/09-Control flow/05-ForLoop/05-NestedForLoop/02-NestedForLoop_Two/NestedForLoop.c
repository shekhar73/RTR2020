#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum, knum;

	//code
	printf("\n\n");

	for (inum = 1; inum <= 10; inum++)
	{
		printf("inum = %d\n", inum);
		printf("--------\n\n");

		for (jnum = 1; jnum <= 5; jnum++)
		{
			printf("\tjnum = %d\n", jnum);
			printf("\t--------\n\n");

			for (knum = 1; knum <= 3; knum++)
			{
				printf("\t\tknum = %d\n", knum);
			}
			printf("\n\n");
		}
		printf("\n\n");
	}
	return(0);
}