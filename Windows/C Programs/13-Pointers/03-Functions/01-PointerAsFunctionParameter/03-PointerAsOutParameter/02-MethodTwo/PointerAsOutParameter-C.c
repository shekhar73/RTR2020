#include <stdio.h>

int main(void)
{
	//function declarations
	void MathematicalOperations(int, int, int *, int *, int *, int *, int *);
	
	//variable declarations
	int iNum1;
	int iNum2;
	int *iAnswerSum;
	int *iAnswerDifference;
	int *iAnswerMultiplication;
	int *iAnswerQuotient;
	int *iAnserRemainder;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a value of iNum1 : ");
	scanf("%d", &iNum1);
	
	printf("\n\n");
	printf("\tEnter a value of iNum2 : ");
	scanf("%d", &iNum2);
	
	iAnswerSum = (int *)malloc(1 * sizeof(int));
	if(iAnswerSum == NULL)
	{
		printf("\tCould not allocate memory for iAnswerSum. Exiting now...!!!\n\n");
		exit(0);
	}
	
	iAnswerDifference = (int *)malloc(1 * sizeof(int));
	if(iAnswerSum == NULL)
	{
		printf("\tCould not allocate memory for iAnswerDifference. Exiting now...!!!\n\n");
		exit(0);
	}
	
	iAnswerMultiplication = (int *)malloc(1 * sizeof(int));
	if(iAnswerSum == NULL)
	{
		printf("\tCould not allocate memory for iAnswerMultiplication. Exiting now...!!!\n\n");
		exit(0);
	}
	
	iAnswerQuotient = (int *)malloc(1 * sizeof(int));
	if(iAnswerSum == NULL)
	{
		printf("\tCould not allocate memory for iAnswerQuotient. Exiting now...!!!\n\n");
		exit(0);
	}
	
	iAnserRemainder = (int *)malloc(1 * sizeof(int));
	if(iAnswerSum == NULL)
	{
		printf("\tCould not allocate memory for iAnserRemainder. Exiting now...!!!\n\n");
		exit(0);
	}
	 
	
	MathematicalOperations(iNum1, iNum2, iAnswerSum, iAnswerDifference, iAnswerMultiplication,
	iAnswerQuotient, iAnserRemainder);
	
	printf("\n\n");
	
	printf("\t***********************************RESULTS**********************************\n\n");
	
	printf("\tSum = %d\n", *iAnswerSum);
	printf("\tDifference = %d\n", *iAnswerDifference);
	printf("\tMultiplication = %d\n", *iAnswerMultiplication);
	printf("\tQuotient = %d\n", *iAnswerQuotient);
	printf("\tRemainder = %d\n", *iAnserRemainder);
	
	
	if(iAnserRemainder)
	{
		free(iAnserRemainder);
		iAnserRemainder = NULL;
		printf("\tMemory allocated for iAnserRemainder successfully freed!!!\n\n");
	}
	
	if(iAnswerQuotient)
	{
		free(iAnswerQuotient);
		iAnswerQuotient = NULL;
		printf("\tMemory allocated for iAnswerQuotient successfully freed!!!\n\n");
	}
	
	if(iAnswerMultiplication)
	{
		free(iAnswerMultiplication);
		iAnswerMultiplication = NULL;
		printf("\tMemory allocated for iAnswerMultiplication successfully freed!!!\n\n");
	}
	
	if(iAnswerDifference)
	{
		free(iAnswerDifference);
		iAnswerDifference = NULL;
		printf("\tMemory allocated for iAnswerDifference successfully freed!!!\n\n");
	}
	
	if(iAnswerSum)
	{
		free(iAnswerSum);
		iAnswerSum = NULL;
		printf("\tMemory allocated for iAnswerSum successfully freed!!!\n\n");
	}
	
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