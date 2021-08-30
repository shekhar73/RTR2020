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
		printf("You Are Eligible For Voting !!!\n\n");
	}
	printf("You Are NOT Eligible For Voting !!!\n\n");
	return(0);
}