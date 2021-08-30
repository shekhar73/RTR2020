#include <stdio.h> // for printf()
#include <conio.h> // for getch()
int main(void)
{
	//variable declarations
	int int_a, int_b;
	int int_result;
	char char_option, char_option_division;

	//code
	printf("\n\n");

	printf("Enter Value For 'A' : ");
	scanf("%d", &int_a);

	printf("Enter Value For 'B' : ");
	scanf("%d", &int_b);

	printf("Enter Option In Character : \n\n");
	printf("'A' or 'a' For Addition : \n");
	printf("'S' or 's' For Subtraction : \n");
	printf("'M' or 'm' For Multiplication : \n");
	printf("'D' or 'd' For Division : \n\n");

	printf("Enter Option : ");
	char_option = getch();

	printf("\n\n");

	if (char_option == 'A' || char_option == 'a')
	{
		int_result = int_a + int_b;
		printf("Addition Of A = %d And B = %d Gives Result %d !!!\n\n", int_a, int_b, int_result);
	}
	else if (char_option == 'S' || char_option == 's')
	{
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
	}
	else if(char_option == 'M' || char_option == 'm')
	{
		int_result = int_a * int_b;
		printf("Multiplication Of A = %d And B = %d Gives Result %d !!!\n\n", int_a, int_b, int_result);
	}
	else if (char_option == 'D' || char_option == 'd')
	{
		printf("Enter Option In Character : \n\n");
		printf("'Q' or 'q' or '/' For Quotient Upon Division : \n");
		printf("'R' or 'r' or '%%' For Remainder Upon Division : \n");
		
		printf("Enter Option : ");
		char_option_division = getch();
		
		printf("\n\n");
		
		if (char_option_division == 'Q' || char_option_division == 'q' || char_option_division == '/')
		{
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
		}
		else if (char_option_division == 'R' || char_option_division == 'r' || char_option_division == '%')
		{
			if (int_a >= int_b)
			{
				int_result = int_a % int_b;
				printf("Division Of A = %d By B = %d Gives Remainder = %d !!!\n\n", int_a, int_b, int_result);
			}
			else
			{		
				int_result = int_b % int_a;
				printf("Division Of B = %d By A = %d Gives Remainder = %d !!!\n\n", int_b, int_a, int_result);
			}
		}
		else
			printf("Invalid Character %c Entered For Division !!! Please Try Again...\n\n", char_option_division);
	}
	else
		printf("Invalid Character %c Entered !!! Please Try Again...\n\n",char_option);
	printf("If - Else If - Else Ladder Complete !!!\n");
	return(0);
}