#include <stdio.h>
int main(void)
{
	//variable declarations
	int icnt, jcnt;

	//code
	printf("\n\n");
	printf("Printing digits from 1 to 10 and 10 to 100: \n\n");
	icnt = 1;
	jcnt = 10;
	do
	{
		printf("\t %d \t %d\n", icnt, jcnt);
		icnt++;
		jcnt = jcnt + 10;
	}while (icnt <= 10, jcnt <= 100);
	printf("\n\n");
	return(0);
}