#include <stdio.h>
int main(void)
{
	//variable declarations
	int i_num, j_num;
	
	//code
	printf("\n\n");
	
	for (i_num = 1; i_num <= 10; i_num++)
	{
		printf("i_num = %d\n", i_num);
		printf("------------------\n\n");
		for (j_num = 1; j_num <= 5; j_num++)
		{
			printf("\tj_num = %d\n", j_num);
		}
		printf("\n\n");
	}
	
	return(0);
}