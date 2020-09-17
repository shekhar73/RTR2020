// Swap numbers without using temporary variable.

#include<stdio.h>

int main(void)
{
	//5, 7

	int iNum1, iNum2 = 0;

	printf("Enter two numbers which you want to swap : \n");
	scanf("%d %d", &iNum1, &iNum2);

	iNum1 = iNum1 + iNum2;
	iNum2 = iNum1 - iNum2;
	iNum1 = iNum1 - iNum2;

	/*iNum1 = iNum1 * iNum2;
	iNum2 = iNum1 / iNum2;
	iNum1 = iNum1 / iNum2;*/


	// iNum1 = iNum1 ^ iNum2;
	// iNum2 = iNum1 ^ iNum2;
	// iNum1 = iNum1 ^ iNum2;


	printf("Numbers after swapping are : iNum1 = %d and iNum2 = %d ", iNum1, iNum2);

	return (0);
}