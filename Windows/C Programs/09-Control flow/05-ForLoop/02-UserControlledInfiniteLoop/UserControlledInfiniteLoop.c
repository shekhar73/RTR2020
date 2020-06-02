#include <stdio.h>
int main(void)
{
	//variable declarations
	char char_option, char_ch = '\0';

	//code
	printf("\n\n");
	printf("Once the infinite loop begins, Enter 'Q' or 'q' to Quit the infinite for loop : \n\n");
	printf("Enter 'Y' oy 'y' to initiate User Controlled infinite loop : ");
	printf("\n\n");

	char_option = getch();

	if (char_option == 'Y' || char_option == 'y')
	{
		for (;;) //Infinite Loop
		{
			printf("In Loop...\n");
			char_ch = getch();
			if (char_ch == 'Q' || char_ch == 'q')
				break; //User Controlled Exitting From Infinite Loop
		}
	}
	printf("\n\n");
	printf("EXITTING USER CONTROLLED INFINITE LOOP...");
	printf("\n\n");
	return(0);
}