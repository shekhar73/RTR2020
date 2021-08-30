#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum, knum;

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
			printf("\t--------\n\n");
			knum = 1;
			do
			{
				printf("\t\tknum = %d\n", knum);
				knum++;
			}while (knum <= 3);
			printf("\n\n");
			jnum++;
		}while (jnum <= 5);
		printf("\n\n");
		inum++;
	}while (inum <= 10);
	
	return(0);
}