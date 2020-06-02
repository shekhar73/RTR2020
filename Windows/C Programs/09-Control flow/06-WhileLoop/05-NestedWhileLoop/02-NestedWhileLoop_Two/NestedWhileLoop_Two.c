#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum, knum;

	//code
	printf("\n\n");
	inum = 1;
	while (inum <= 10)
	{
		printf("inum = %d\n", inum);
		printf("--------\n\n");
		jnum = 1;
		while (jnum <= 5)
		{
			printf("\tjnum = %d\n", jnum);
			printf("\t--------\n\n");
			knum = 1;
			while (knum <= 3)
			{
				printf("\t\tk = %d\n", knum);
				knum++;
			}
			printf("\n\n");
			jnum++;
		}
		printf("\n\n");
		inum++;
	}
	return(0);
}