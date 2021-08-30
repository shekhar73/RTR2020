// Print number line for the given number.
	/*eg: input.   : 4
	      output  : -4 -3 -2 -1 0 1 2 3 4*/

#include <stdio.h>

int main(void)
{
	int iNum = 0, i = 0, iTemp = 0;

	printf("Enter the number which you want to print as a Number Line : ");
	scanf("%d", &iNum);

    iTemp = iNum;
	iTemp = iTemp * (-1);

	for(i = 0; i <= (iNum * 2); i++)
	{
		printf("%d\t", iTemp + i);
	}
	return (0);
}
