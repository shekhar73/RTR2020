#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum, cnum;

	//code
	printf("\n\n");

	for (inum = 0; inum < 64; inum++)
	{
		for (jnum = 0; jnum < 64; jnum++)
		{
			cnum = ((inum & 0x8) == 0) ^ ((jnum & 0x8) == 0);
			
			if (cnum == 0)
				printf(" ");
			
			if (cnum == 1)
				printf("* ");
		}
		printf("\n\n");
	}
	return(0);
}