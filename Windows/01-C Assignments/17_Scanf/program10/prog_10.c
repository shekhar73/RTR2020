

/*
 *
 *
 *	10.	Write a C Program, Take Initials of your name, age, salary, phone number and print them.
 *	
 *	Input/Output Constraints.
 *	Input:
 *		M
 *		S
 *		T
 *		23
 *		111111.1111
 *		12345678890
 *
 *	Output:
 *		M.S.T, 23, 111111.1111, 1234567890
 *
 */


#include <stdio.h>

int main(void)
{
	char chValue1, chValue2, chValue3;
	int iAge = 0;
	float fSalary = 0;
	long double lPhoneNumber = 0.00;

	printf("\nEnter your Initial of First Name, Middle Name, Last Name, Age, Salary, Phone Number : ");
	scanf(" %c %c %c %d %f %le", &chValue1, &chValue2, &chValue3, &iAge, &fSalary, &lPhoneNumber);
	
	printf("\n");
	printf(" %c.%c.%c, %d, %f, %le", chValue1, chValue2, chValue3, iAge, fSalary, lPhoneNumber);
	printf("\n");
	
	return (0);
}

