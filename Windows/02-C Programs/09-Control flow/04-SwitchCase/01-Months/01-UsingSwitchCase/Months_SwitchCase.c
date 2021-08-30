#include <stdio.h>
int main(void)
{
	//variable declarations
	int int_num_month;

	//code
	printf("\n\n");
	printf("Enter Number Of Month (1 to 12) : ");
	scanf("%d", &int_num_month);

	printf("\n\n");

	switch (int_num_month)
	{
		case 1: //like 'if'
			printf("Month number %d is JANUARY !!!\n\n", int_num_month);
			break;
		case 2: //like 'else if'
			printf("Month number %d is FEBRUARY !!!\n\n", int_num_month);
			break;
		case 3: //like 'else if'
			printf("Month number %d is MARCH !!!\n\n", int_num_month);
			break;
		case 4: //like 'else if'
			printf("Month number %d is APRIL !!!\n\n", int_num_month);
			break;
		case 5: //like 'else if'
			printf("Month number %d is MAY !!!\n\n", int_num_month);
			break;
		case 6: //like 'else if'
			printf("Month number %d is JUNE !!!\n\n", int_num_month);
			break;
		case 7: //like 'else if'
			printf("Month number %d is JULY !!!\n\n", int_num_month);
			break;
		case 8: //like 'else if'
			printf("Month number %d is AUGUST !!!\n\n", int_num_month);
			break;
		case 9: //like 'else if'
			printf("Month number %d is SEPTEMBER !!!\n\n", int_num_month);
			break;
		case 10: //like 'else if'
			printf("Month number %d is OCTOBER !!!\n\n", int_num_month);
			break;
		case 11: //like 'else if'
			printf("Month number %d is NOVEMBER !!!\n\n", int_num_month);
			break;
		case 12: //like 'else if'
			printf("Month number %d is DECEMBER !!!\n\n", int_num_month);
			break;
		default: //like ending OPTIONAL 'else'...just like terminating 'else' is optional in if-else if-else ladder, so is the 'default' case optional in switch-case
			printf("Invalid Month number %d entered !!! Please try again later...\n\n", int_num_month);
		break;
	}
	printf("Switch case block completed !!!\n");
	return(0);
}