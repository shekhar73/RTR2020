
/*
 *
 *
 *	9.	Write a C Program, which takes three input in a SINGLE LINE and print as following.
 *
 *
 *	Input/Output Constraints:
 *
 *	Input:	M 
 *		10 
 *		21.110
 *	
 *	Output:	M = 10 * 21.110
 *		M = 211.1;	 
 *
 */


#include <stdio.h>

int main(void)
{
	float fNumber1 = 0, fNumber2 = 0, fResult = 0.0f;
	char chValue = 0.0f;

	printf("\nEnter space seperated 3 values as character, float, float : ");
	scanf("%c %f %f", &chValue, &fNumber1, &fNumber2);
	printf(" %c = %f * %f\n", chValue, fNumber1, fNumber2);
	printf(" %c = %f\n\n", chValue, fNumber1 * fNumber2);
	
	return (0);

}

