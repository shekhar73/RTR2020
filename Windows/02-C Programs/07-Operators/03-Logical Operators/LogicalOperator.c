#include <stdio.h>

int main(void)
{
	int sk_ia, sk_ib, sk_ic, sk_iresult;

	printf("\n\n");
	printf("Enter First Number : ");
	scanf("%d", &sk_ia);

	printf("\n\n");
	printf("Enter Second Number : ");
	scanf("%d", &sk_ib);

	printf("\n\n");
	printf("Enter Third Number : ");
	scanf("%d", &sk_ic);

	printf("\n\n");
	printf("If Answer = 0, It Is 'FALSE'.\n");
	printf("If Answer = 1, It Is 'TRUE'.\n\n");

	sk_iresult = (sk_ia <= sk_ib) && (sk_ib != sk_ic);
	printf("LOGICAL AND (&&) : Answer is TRUE (1) IFF Both conditons are TRUE. The Answer is FALSE (0) if any one or both conditons are FALSE.\n\n");
	printf("A = %d is less than or equal to B = %d and B = %d is NOT EQUAL to C = %d \t Answer = %d\n\n", sk_ia, sk_ib, sk_ib, sk_ic, sk_iresult);


	sk_iresult = (sk_ib >= sk_ia) || (sk_ia == sk_ic);
	printf("LOGICAL OR (||) : Answer is FALSE (0) IFF Both conditons are FALSE. The Answer is TRUE (1) if any one or both conditons are TRUE.\n\n");
	printf("Either B = %d is greater than or equal to A = %d or A = %d is Equal to C = %d \t Answer = %d\n\n", sk_ib, sk_ia, sk_ia, sk_ic, sk_iresult);

	sk_iresult = !sk_ia;
	printf("A = %d and Using Logical NOT (!) Operator on A Gives Result = %d\n\n", sk_ia, sk_iresult);

	sk_iresult = !sk_ib;
	printf("B = %d And Using Logical NOT (!) Operator on B Gives Result = %d\n\n", sk_ib, sk_iresult);

	sk_iresult = !sk_ic;
	printf("C = %d And Using Logical NOT (!) Operator on C Gives Result = %d\n\n", sk_ic, sk_iresult);

	sk_iresult = (!(sk_ia <= sk_ib) && !(sk_ib != sk_ic));
	printf("Using Logical NOT (!) On (sk_ia <= sk_ib) And Also On (sk_ib != sk_ic) And then ANDing Them Afterwards Gives Result = %d\n", sk_iresult);
	
	printf("\n\n");

	sk_iresult = !((sk_ib >= sk_ia) || (sk_ia == sk_ic));
	printf("Using Logical NOT (!) On Entire Logical Expression (sk_ib >= sk_ia) || (sk_ia ==sk_ic) Gives Result = %d\n", sk_iresult);
	printf("\n\n");

	return(0);

}