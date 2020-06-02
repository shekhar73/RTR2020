#include <stdio.h>
#include <conio.h>
int main(void)
{
	//variable declarations
	int inum, jnum;

	//code
	printf("\n\n");

	for (inum = 1; inum <= 20; inum++)
	{
		for (jnum = 1; jnum <= 20; jnum++)
		{
			if (jnum > inum)
			{
				break;
			}
			else
			{
				printf("* ");
			}
		}
		printf("\n");
	}
	printf("\n\n");
	return(0);
}