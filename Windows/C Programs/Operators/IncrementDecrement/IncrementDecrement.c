#include <stdio.h>

int main(void)
{
	//variable declarations
	int sk_ia = 7;
	int sk_ib = 18;

	//code
	printf("\n\n");
	printf("A = %d\n", sk_ia);
	printf("A = %d\n", sk_ia++);
	printf("A = %d\n", sk_ia);
	printf("A = %d\n\n", ++sk_ia);

	printf("B = %d\n", sk_ib);
	printf("B = %d\n", sk_ib--);
	printf("B = %d\n", sk_ib);
	printf("B = %d\n\n", --sk_ib);

	return(0);
}
