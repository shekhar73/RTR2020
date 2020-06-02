#include <stdio.h>
int main(void)
{
	//variable declarations
	int my_age;

	//code
	printf("\n\n");
	printf("Enter your Age : ");
	scanf("%d", &my_age);
	
	printf("\n\n");
	if (my_age >= 18)
	{
		printf("Entering if-block...\n\n");
		printf("You Are Eligible For Voting !!!\n\n");
	}
	else
	{
		printf("Entering else-block...\n\n");
		printf("You Are NOT Eligible For Voting !!!\n\n");
	}
	
	printf("Bye !!!\n\n");
	return(0);
}