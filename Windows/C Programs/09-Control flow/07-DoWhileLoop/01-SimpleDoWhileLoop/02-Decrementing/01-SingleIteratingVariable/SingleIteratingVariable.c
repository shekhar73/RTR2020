#include <stdio.h>
int main(void)
{
	//variable declarations
	int icnt;

	//code
	printf("\n\n");
	printf("Printing digits 10 to 1 : \n\n");
	icnt = 10;
	do
	{
		printf("\t%d\n", icnt);
		icnt--;
	}while (icnt >= 1);
	printf("\n\n");
	return(0);
}