#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int data;
	int choice;

	do
	{
		printf("Enter an integer:");
		scanf("%d", &data);
		printf("Entered data = %d\n", data);
		printf("Do you want to enter yet another integer:[1->Yes|No otherwise]:");
		scanf("%d", &choice);
	}
	while(choice == 1);

	return (EXIT_SUCCESS);
}