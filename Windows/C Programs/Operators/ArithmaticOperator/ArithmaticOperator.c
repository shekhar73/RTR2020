#include <stdio.h>

int main(void)
{
	//variable declarations
	int sk_inuma, sk_inumb, sk_iresult;

	//code
	printf("\n\n");
	printf("Enter A Number : ");
	scanf("%d", &sk_inuma);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &sk_inumb);

	printf("\n\n");

	// *** The Following are the 5 arithmetic operators +, -, *, / and % ***
	// *** Also, The Resultants of the arithmetic operations in all the Below 
	//Five cases have been assigned to the variable 'result' using the assignment operator (=) ***

	sk_iresult = sk_inuma + sk_inumb;
	printf("Addition Of A = %d and B = %d Gives %d.\n", sk_inuma, sk_inumb, sk_iresult);

	sk_iresult = sk_inuma - sk_inumb;
	printf("Subtraction Of A = %d and B = %d Gives %d.\n", sk_inuma, sk_inumb, sk_iresult);

	sk_iresult = sk_inuma * sk_inumb;
	printf("Multiplication Of A = %d and B = %d Gives %d.\n", sk_inuma, sk_inumb, sk_iresult);

	sk_iresult = sk_inuma / sk_inumb;
	printf("Division Of A = %d and B = %d Gives Quotient %d.\n", sk_inuma, sk_inumb, sk_iresult);

	sk_iresult = sk_inuma % sk_inumb;
	printf("Division Of A = %d and B = %d Gives Remainder %d.\n", sk_inuma, sk_inumb, sk_iresult);

	printf("\n\n");

	return(0);
}
