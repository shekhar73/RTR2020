#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum, cnum;

	//code
	printf("\n\n");
	inum = 0;
	do
	{
		jnum = 0;
		do
		{
			cnum = ((inum & 0x8) == 0) ^ ((jnum & 0x8) == 0);
			if (cnum == 0)
				printf(" ");
			if (cnum == 1)
				printf("* ");
			jnum++;
		}while (jnum < 64);
		printf("\n\n");
		inum++;
	}while (inum < 64);
	
	return(0);
}