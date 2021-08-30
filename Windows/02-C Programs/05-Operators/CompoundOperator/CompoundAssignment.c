#include <stdio.h>

int main(void)
{
	//variable declarations
	int sk_ia, sk_ib, sk_ix;

	//code
	printf("\n\n");
	printf("Enter A Number : ");
	scanf("%d", &sk_ia);

	printf("\n\n");
	printf("Enter Another Number : ");
	scanf("%d", &sk_ib);

	printf("\n\n");

	//Since, In all the following 5 Cases, the operand on the left 'sk_ia' is getting repeated immeiately on the right (e.g : sk_ia = sk_ia + b or sk_ia = sk_ia -b),
	//we are using compound assignment operators +=, -=, *=, /= and %=

	//Since, 'a' will be assigned the value of (sk_ia + sk_ib) at the expression (sk_ia += sk_ib), we must save the original value of 'sk_ia' to another variable (sk_ix)
	sk_ix = sk_ia;
	sk_ia += sk_ib; // sk_ia = sk_ia + sk_ib
	printf("Addition Of A = %d and B = %d Gives %d.\n", sk_ix, sk_ib, sk_ia);

	//Value Of 'sk_ia' Altered In The Above Expression Is Used Here...
	//Since, 'sk_ia' Will Be Assigned The Value Of (sk_ia - sk_ib) At The Expression (sk_ia -= sk_ib), We Must Save The Original Value Of 'sk_ia' To Another Variable (sk_ix)
	sk_ix = sk_ia;
	sk_ia -= sk_ib; // sk_ia = sk_ia - sk_ib
	printf("Subtraction Of A = %d And B = %d Gives %d.\n", sk_ix, sk_ib, sk_ia);

	//Value Of 'sk_ia' Altered In The Above Expression Is Used Here...
	//Since, 'sk_ia' Will Be Assigned The Value Of (sk_ia * sk_ib) At The Expression (sk_ia *= sk_ib), We Must Save The Original Value Of 'sk_ia' To Another Variable (sk_ix)
	sk_ix = sk_ia;
	sk_ia *= sk_ib; //sk_ia = sk_ia * sk_ib
	printf("Multiplication Of A = %d And B = %d Gives %d.\n", sk_ix, sk_ib, sk_ia);

	//Value Of 'sk_ia' Altered In The Above Expression Is Used Here...
	//Since, 'sk_ia' Will Be Assigned The Value Of (sk_ia / sk_ib) At The Expression (sk_ia /= sk_ib), We Must Save The Original Value Of 'sk_ia' To Another Variable (sk_ix)
	sk_ix = sk_ia;
	sk_ia /= sk_ib; // sk_ia = sk_ia / sk_ib
	printf("Division Of A = %d And B = %d Gives Quotient %d.\n", sk_ix, sk_ib, sk_ia);

	//Value Of 'sk_ia' Altered In The Above Expression Is Used Here...
	//Since, 'sk_ia' Will Be Assigned The Value Of (sk_ia % sk_ib) At The Expression (sk_ia %= sk_ib), We Must Save The Original Value Of 'sk_ia' To Another Variable (sk_ix)
	sk_ix = sk_ia;
	sk_ia %= sk_ib; // sk_ia = sk_ia % sk_ib
	printf("Division Of A = %d And B = %d Gives Remainder %d.\n", sk_ix, sk_ib, sk_ia);

	printf("\n\n");

	return(0);
}
