#include <stdio.h>
int main(void)
{
	//variable declarations
	int i_num1 = 0, i_num2 = 0, i_num3 = 0;

	//code
	printf("\n\n");
	printf("Enter an integer value from which Iteration must begin : ");
	scanf("%d", &i_num1);

	printf("How may digits do you want to ptint from %d Onwards ? : ", i_num2);
	scanf("%d", &i_num2);

	printf("Printing Digits %d to %d : \n\n", i_num1, (i_num2 + i_num3));

	for (i_num3 = i_num1; i_num3 <= (i_num1 + i_num2); i_num3++)
	{
		printf("\t%d\n", i_num3);
	}

	printf("\n\n");
	return(0);
}