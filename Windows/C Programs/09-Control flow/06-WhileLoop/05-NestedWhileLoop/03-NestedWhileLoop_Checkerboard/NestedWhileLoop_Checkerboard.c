#include <stdio.h>
int main(void)
{
	//variable declarations
	int icnt, jcnt, ccnt;

	//code
	printf("\n\n");
	icnt = 0;
	while (icnt < 64)
	{
		jcnt = 0;
		while (jcnt < 64)
		{
			ccnt = ((icnt & 0x8) == 0) ^ ((jcnt & 0x8) == 0);
			if (ccnt == 0)
				printf(" ");
			if (ccnt == 1)
				printf("* ");
			jcnt++;
		}
		printf("\n\n");
		icnt++;
	}
	return(0);
}