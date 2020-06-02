#include <stdio.h>
int main(void)
{
	//variable declarations
	int i_num1 = 0, i_num2 = 0, icnt = 0;

	//code
	printf("\n\n");

	printf("Enter an Integer Value from Which Iteration Must Begin : ");
	scanf("%d", &i_num1);

	printf("How many Digits do you want to Print from %d Onwards ? : ", i_num2);
	scanf("%d", &i_num2);

	printf("Printing Digits %d to %d : \n\n", i_num1, (i_num1 + i_num2));

	icnt = i_num1;
	while (icnt <= (i_num1 + i_num2))
	{
		printf("\t%d\n", icnt);
		icnt++;
	}
	printf("\n\n");
	return(0);
}