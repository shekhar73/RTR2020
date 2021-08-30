#include <stdio.h>
int main(void)
{
	//variable declarations
	int inum, icnt;

	//code
	printf("\n\n");
	printf("Printing the digits from 1 to 10 : \n\n");

	icnt = 1;
	while (icnt <= 10)
	{
		printf("\t%d\n", icnt);
		icnt++;
	}
	printf("\n\n");
	return(0);
}