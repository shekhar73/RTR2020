#include <stdio.h>

int main(void)
{
	int sk_inum1, sk_inum2;
	int sk_p, sk_q;
	char sk_ch1, sk_ch2;
	int sk_iresult1, sk_iresult2;

	printf("\n\n");

	sk_inum1 = 5;
	sk_inum2 = 7;
	sk_ch1 = (sk_inum1 > sk_inum2) ? 'A' : 'B';
	sk_iresult1 = (sk_inum1 > sk_inum2) ? sk_inum1 : sk_inum2;

	printf("Ternary Operator Answer 1 -----%c and %d.\n\n", sk_ch1, sk_iresult1);

	sk_p = 30;
	sk_q = 30;
	sk_ch2 = (sk_p != sk_q) ? 'P' : 'Q';
	sk_iresult2 = (sk_p != sk_q) ? sk_p : sk_q;

	printf("Ternary Operator Answer 2 -----%c and %d.\n\n", sk_ch2, sk_iresult2);

	printf("\n\n");
	return (0);
}