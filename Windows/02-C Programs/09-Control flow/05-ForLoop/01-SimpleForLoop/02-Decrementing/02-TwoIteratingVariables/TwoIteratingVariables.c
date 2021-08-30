#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");
	printf("Printing the digits from 1 to 10 and 10 to 100 : \n\n");
	for (inum = 10, jnum = 100; inum >= 1, jnum >= 10; inum--, jnum = jnum - 10)
	{
		printf("\t %d \t %d\n", inum, jnum);
	}
	printf("\n\n");
	return(0);
}