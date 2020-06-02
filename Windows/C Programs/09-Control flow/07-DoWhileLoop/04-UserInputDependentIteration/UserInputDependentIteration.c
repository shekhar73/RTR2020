#include <stdio.h>
int main(void)
{
	//variable declarations
	int i_num1, num1, i1;

	//code
	printf("\n\n");

	printf("Enter an integer value from which iterration must begin : ");
	scanf("%d", &i_num1);

	printf("How many digits do you want to print from %d onwards ? : ", num1);
	scanf("%d", &num1);

	printf("Printing Digits from %d to %d : \n\n", i_num1, (i_num1 + num1));
	i1 = i_num1;

	do
	{
		printf("\t%d\n", i1);
		i1++;
	}while (i1 <= (i_num1 + num1));
	printf("\n\n");
	return(0);
}