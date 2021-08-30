#include <stdio.h>
int main(void)
{
	//variable declarations
	char char_option, char_ch = '\0';
	
	//code
	printf("\n\n");
	printf("Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");
	do
	{
		do
		{
			printf("\n");
			printf("In Loop...\n");
			char_ch = getch(); //control flow waits for character input...
		} while (char_ch != 'Q' && char_ch != 'q');
		printf("\n\n");
		printf("Exiting user controlled Infinite loop...");
		printf("\n\n");
		printf("Do you want to begin user controlled infinite loop again?...(Y/y - Yes, Any Other Key - No) : ");
		char_option = getch();
	}while (char_option == 'Y' || char_option == 'y');
	
	return(0);
}