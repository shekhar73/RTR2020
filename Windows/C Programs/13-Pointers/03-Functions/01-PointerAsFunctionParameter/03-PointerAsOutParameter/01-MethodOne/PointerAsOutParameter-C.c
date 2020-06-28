#include <stdio.h>

int main(void)
{
	//function declarations
	void MathematicalOperations(int, int, int *, int *, int *, int *, int *);
	
	//variable declarations
	int iNum1;
	int iNum2;
	int iAnswerSum;
	int iAnswerDifference;
	int iAnswerMultiplication;
	int iAnswerQuotient;
	int iAnserRemainder;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a value of iNum1 : ");
	scanf("%d", &iNum1);
	
	printf("\n\n");
	printf("\tEnter a value of iNum2 : ");
	scanf("%d", &iNum2);
	
	MathematicalOperations(iNum1, iNum2, &iAnswerSum, &iAnswerDifference, &iAnswerMultiplication,
	&iAnswerQuotient, &iAnserRemainder);
	
	printf("\n\n");
	
	printf("\t***********************************RESULTS**********************************\n\n");
	
	printf("\tSum = %d\n", iAnswerSum);
	printf("\tDifference = %d\n", iAnswerDifference);
	printf("\tMultiplication = %d\n", iAnswerMultiplication);
	printf("\tQuotient = %d\n", iAnswerQuotient);
	printf("\tRemainder = %d\n", iAnserRemainder);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return (0);
}

void MathematicalOperations(int num1, int num2, int *sum, int *difference, int *product, int *qutioent, int *remainder)
{
	//code
	
	*sum		= num1 + num2;
	*difference = num1 - num2;
	*product 	= num1 * num2;
	*qutioent 	= num1 / num2;
	*remainder 	= num1 % num2;
}