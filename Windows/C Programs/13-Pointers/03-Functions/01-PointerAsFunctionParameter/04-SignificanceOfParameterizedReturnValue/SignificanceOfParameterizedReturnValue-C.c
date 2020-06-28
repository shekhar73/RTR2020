#include <stdio.h>

enum
{
	NEGATIVE_NO = -1,
	ZERO_NO,
	POSITIVE_NO
};

int main(void)
{
	//function declarations
	int Difference(int, int, int *);
	
	//variable declarations
	int iNum1, iNum2, iAnswer, iResult;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a value of iNum1 : ");
	scanf("%d", &iNum1);
	
	printf("\n\n");
	printf("\tEnter a value of iNum2 : ");
	scanf("%d", &iNum2);
	
	iResult = Difference(iNum1, iNum2, &iAnswer);
	
	printf("\n\n");
	printf("\tDifference of %d and %d = %d\n", iNum1, iNum2, iAnswer);

	if(iResult == POSITIVE_NO)
	{
		printf("\tThe Difference between %d and %d numbers is Positive. \n\n", iNum1, iNum2);
	}
	else if(iResult == NEGATIVE_NO)
	{
		printf("\tThe Difference between %d and %d numbers is Negative. \n\n", iNum1, iNum2);
	}
	else
	{
		printf("\tThe Difference between %d and %d numbers is Zero. \n\n", iNum1, iNum2);
	}
	return (0);
}

int Difference(int iNum1, int iNum2, int *difference)
{
	//code
	*difference = iNum1 - iNum2;
	
	if(*difference > 0)
	{
		return (POSITIVE_NO);
	}
	else if(*difference < 0)
	{
		return (NEGATIVE_NO);
	}
	else
	{
		return (ZERO_NO);
	}
}