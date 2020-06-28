#include <stdio.h>

int main(void)
{
	//function prototype
	void Swapnumbers(int *, int *);
	
	//variable declarations
	int iNum1;
	int iNum2;
	
	//code
	printf("\n\n");
	printf("\tEnter value for iNum1 : ");
	scanf("%d", &iNum1);
	
	printf("\tEnter value for iNum2 : ");
	scanf("%d", &iNum2);
	
	printf("\n\n");
	printf("\t********************Before Swapping********************\n\n");
	printf("\tValue of iNum1 = %d\n\n", iNum1);
	printf("\tValue of iNum2 = %d\n\n", iNum2);
	
	Swapnumbers(&iNum1, &iNum2);
	
	printf("\n\n");
	printf("\t****************After Swapping**********************\n\n");
	printf("\tValue of iNum1 = %d\n\n", iNum1);
	printf("\tValue of iNum2 = %d\n\n", iNum2);

	return (0);
}

void Swapnumbers(int *A, int *B)
{
	int temp;
	
	printf("\t********************Before Swapping********************\n\n");
	printf("\tValue of A = %d\n\n", *A);
	printf("\tValue of B = %d\n\n", *B);
	
	temp = *A;
	*A = *B;
	*B = temp;
	
	printf("\n\n");
	printf("\t****************After Swapping**********************\n\n");
	printf("\tValue of A = %d\n\n", *A);
	printf("\tValue of B = %d\n\n", *B);
}