#include <stdio.h>
int main(void)
{
	//variable declarations
	char char_option, char_ch = '\0';

	//code
	printf("\n\n");
	printf("Once the infinite loop Begins, enter 'Q' or 'q' to quit the infinite for loop : \n\n");
	printf("Enter 'Y' oy 'y' To Initiate User Controlled Infinite Loop : ");

	printf("\n\n");
	char_option = getch();
	
	if (char_option == 'Y' || char_option == 'y')
	{
		while (1) //Infinite Loop
		{
			printf("In Loop...\n");
			char_ch = getch();

			if (char_ch == 'Q' || char_ch == 'q')
				break; //User Controlled Exitting From Infinite Loop
		}
		printf("\n\n");
		printf("EXITTING USER CONTROLLED INFINITE LOOP...");
		printf("\n\n");
	}
	else
	printf("You must rress 'Y' or 'y' to Initiate the User controlled infinite loop....Please Try Again...\n\n");
	return(0);
}