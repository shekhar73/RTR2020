#include <stdio.h>
#include <conio.h>
int main(void)
{
	//variable declarations
	int int_a, int_b;
	int int_result;
	char char_option, char_option_division;

	//code
	printf("\n\n");

	printf("Enter Value for 'A' : ");
	scanf("%d", &int_a);

	printf("Enter Value for 'B' : ");
	scanf("%d", &int_b);

	printf("Enter Option in Character : \n\n");
	printf("'A' or 'a' For Addition : \n");
	printf("'S' or 's' For Subtraction : \n");
	printf("'M' or 'm' For Multiplication : \n");
	printf("'D' or 'd' For Division : \n\n");

	printf("Enter Option : ");
	char_option = getch();

	printf("\n\n");

	switch (char_option)
	{
		// FALL THROUGH CONSITION FOR 'A' and 'a'
		case 'A':
		case 'a':
			int_result = int_a + int_b;
			printf("Addition Of A = %d And B = %d Gives Result %d !!!\n\n", int_a, int_b, int_result);
			break;
		// FALL THROUGH CONSITION FOR 'S' and 's'
		case 'S':
		case 's':
			if (int_a >= int_b)
			{
				int_result = int_a - int_b;
				printf("Subtraction Of B = %d From A = %d Gives Result %d !!!\n\n", int_b, int_a, int_result);
			}
			else
			{
				int_result = int_b - int_a;
				printf("Subtraction Of A = %d From B = %d Gives Result %d !!!\n\n", int_a, int_b, int_result);
			}
			break;
	
			// FALL THROUGH CONSITION FOR 'M' and 'm'
		case 'M':
		case 'm':
			int_result = int_a * int_b;
			printf("Multiplication Of A = %d And B = %d Gives Result %d !!!\n\n", int_a, int_b, int_result);
			break;
			
			// FALL THROUGH CONSITION FOR 'D' and 'd'
		case 'D':
		case 'd':
			printf("Enter Option In Character : \n\n");
			printf("'Q' or 'q' or '/' For Quotient Upon Division : \n");
			printf("'R' or 'r' or '%%' For Remainder Upon Division : \n");
			
			printf("Enter Option : ");
			char_option_division = getch();
			
			printf("\n\n");
			
			switch (char_option_division)
			{
				// FALL THROUGH CONSITION FOR 'Q' and 'q' and '/'
				case 'Q':
				case 'q':
				case '/':
					if (int_a >= int_b)
					{
						int_result = int_a / int_b;
						printf("Division Of A = %d By B = %d Gives Quotient = %d !!!\n\n", int_a, int_b, int_result);
					}
					else
					{
						int_result = int_b / int_a;
						printf("Division Of B = %d By A = %d Gives Quotient = %d !!!\n\n", int_b, int_a, int_result);
					}
					break; // 'break' of case 'Q' or case 'q' or case '/'
					// FALL THROUGH CONSITION FOR 'R' and 'r' and '%'
				case 'R':
				case 'r':
				case '%':
					if (int_a >= int_b)
					{
						int_result = int_a % int_b;
						printf("Division Of A = %d By B = %d Gives Remainder = %d !!!\n\n", int_a, int_b, int_result);
					}
					else
					{
						int_result = int_b % int_a;
						printf("Division Of B = %d By A = %d Gives Remainder = %d !!!\n\n", int_a, int_b, int_result);
					}
					break; // 'break' of case 'R' or case 'r' or case '%'
				default: // 'default' case for switch(option_division)
					printf("Invalid Character %c Entered For Division !!! Please Try Again...\n\n", char_option_division);
					break; // 'break' of 'default' of switch(option_division)
			} // ending curly brace of switch(option_division)
			break; // 'break' of case 'D' or case 'd'
		
		default: // 'default' case for switch (option)
			printf("Invalid Character %c Entered !!! Please Try Again...\n\n",char_option);
			break;		
	} // ending curly brace of switch(option)
	printf("Switch Case Block Complete !!!\n");
	return(0);
}