#include <stdio.h>
int main(void)
{
	//variable declarations
	int myage;

	//code
	printf("\n\n");
	printf("Enter your Age : ");
	scanf("%d", &myage);

	if (myage >= 18)
	{
		printf("You Are Eligible For Voting !!!\n");
		printf("Thank you (:\n");
	}
	return(0);
}