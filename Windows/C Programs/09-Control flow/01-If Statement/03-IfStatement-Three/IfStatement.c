#include <stdio.h>
int main(void)
{
	//variable declarations
	int i_num;

	//code
	printf("\n\n");
	printf("Enter Value For 'i_num' : ");
	scanf("%d", &i_num);

	if (i_num < 0)
	{
		printf("Entered Number = %d Is Less Than 0 (NEGATIVE).\n\n", i_num);
	}
	if ((i_num > 0) && (i_num <= 100))
	{
		printf("Entered Number = %d Is In-between 0 And 100.\n\n", i_num);
	}
	if ((i_num > 100) && (i_num <= 200))
	{
		printf("Entered Number = %d Is Between 101 And 200.\n\n", i_num);
	}
	if ((i_num > 200) && (i_num <= 300))
	{
		printf("Entered Number = %d Is Between 201 And 300.\n\n", i_num);
	}
	if ((i_num > 300) && (i_num <= 400))
	{
		printf("Entered Number = %d Is Between 301 And 400.\n\n", i_num);
	}
	if ((i_num > 400) && (i_num <= 500))
	{
		printf("Entered Number = %d Is Between 401 And 500.\n\n", i_num);
	}
	if (i_num > 500)
	{
		printf("Entered Number = %d Is Greater Than 500.\n\n", i_num);
	}
	return(0);
}