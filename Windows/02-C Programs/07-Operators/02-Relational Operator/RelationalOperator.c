#include <stdio.h>

int main(void)
{
	//variable declarations
	int sk_a, sk_b, sk_result;

	//code
	printf("\n\n");
	printf("Enter first Integer : ");
	scanf("%d", &sk_a);

	printf("\n\n");
	printf("Enter second Integer");
	scanf("%d", &sk_b);

	printf("\n\n");
	printf("If Answer = 0, It Is 'FALSE'.\n");
	printf("If Answer = 1, It Is 'TRUE'.\n\n");

	sk_result = (sk_a < sk_b);
	printf("(sk_a < sk_b) A = %d is Less than B = %d \t Answer = %d\n", sk_a, sk_b, sk_result);

	sk_result = (sk_a > sk_b);
	printf("(sk_a > sk_b) A = %d is Greater than B = %d \t Answer = %d\n", sk_a, sk_b, sk_result);

	sk_result = (sk_a <= sk_b);
	printf("(sk_a <= sk_b) A = %d is Less than or Equal to B = %d \t Answer = %d\n", sk_a, sk_b, sk_result);

	sk_result = (sk_a >= sk_b);
	printf("(sk_a >= sk_b) A = %d is Greater than or Equal to B = %d \t Answer = %d\n", sk_a, sk_b, sk_result);

	sk_result = (sk_a == sk_b);
	printf("(sk_a == sk_b) A = %d is Equal to B = %d \t Answer = %d\n", sk_a, sk_b, sk_result);

	sk_result = (sk_a != sk_b);
	printf("(sk_a != sk_b) A = %d is Not Equal to B = %d \t Answer = %d\n", sk_a, sk_b, sk_result);

	return (0);
}