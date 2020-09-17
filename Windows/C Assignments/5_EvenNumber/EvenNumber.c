// Print even number between 1 - 100.

#include <stdio.h>

int main()
{

	int i = 1;
	printf("Even numbers between 1 to 100 are : \n");
	for (i = 1; i < 100; i++)
	{
		if (i % 2 == 0)
		{
			printf("%d\t", i);
		}
	}
	return (0);
}
