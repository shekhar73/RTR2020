#include <stdio.h>
int main(void)
{
	//variable declarations
	int icnt;

	//code
	printf("\n\n");
	printf("Printing digits from 1 to 10 : \n\n");
	icnt = 1;
	do
	{
		printf("\t%d\n", icnt);
		icnt++;
	}while (icnt <= 10);
	
	printf("\n\n");
	return(0);
}