#include <stdio.h>
#include <conio.h>
int main(void)
{
	//variable declarations
	int inum;
	char char_ch;

	//code
	printf("\n\n");

	printf("Printing even numbers from 1 to 100 for every user input. Exiting the loop when user enters character 'Q' or 'q' : \n\n");
	printf("Enter Character 'Q' or 'q' to exit loop : \n\n");

	for (inum = 1; inum <= 10; inum++)
	{
		printf("\t%d\n", inum);
		char_ch = getch();
		
		if (char_ch == 'Q' || char_ch == 'q')
		{
			break;
		}
	}
	printf("\n\n");
	printf("EXITTING LOOP...");
	printf("\n\n");
	return(0);
}