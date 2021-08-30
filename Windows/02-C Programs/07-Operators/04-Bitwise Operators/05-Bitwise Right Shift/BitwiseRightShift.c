#include <stdio.h>

int main(void)
{
	//function prototypes
	void PrintBinaryFormofNumber(unsigned int);

	//vaiable decalrations
	unsigned int sk_ia, sk_num_bits, sk_iresult;

	//code
	printf("\n");
	printf("Enter A Number = ");
	scanf("%u", &sk_ia);

	printf("\n\n");
	
	printf("By how many Bits do you want to Shift A = %d to the Right ? ", sk_ia);
	scanf("%u", &sk_num_bits);

	printf("\n\n");

	sk_iresult = sk_ia >> sk_num_bits;
	printf("Bitwise RIGHT-SHIFTing of A = %d By %d Bits \nGives the result = %d (Decimal).\n\n", sk_ia, sk_num_bits, sk_iresult);
	PrintBinaryFormofNumber(sk_ia);
	PrintBinaryFormofNumber(sk_iresult);

	return (0);
}


void PrintBinaryFormofNumber(unsigned int sk_decimal_number)
{
	//variable declarations
	unsigned int sk_quotient, sk_remainder;
	unsigned int sk_inum;
	unsigned int sk_binary_array[8] = {0};
	int sk_icnt;

	
	printf("The Binary form of the Decimal Integer %d is\t=\t", sk_decimal_number);

	sk_inum = sk_decimal_number;
	sk_icnt = 7;

	while (sk_inum != 0)
	{
		sk_remainder = sk_inum % 2;
		sk_inum = sk_inum / 2;
		sk_binary_array[sk_icnt] = sk_remainder;
		sk_icnt--;
	}

	for (sk_icnt = 0; sk_icnt < 8; sk_icnt++)
	{
		printf("%u", sk_binary_array[sk_icnt]);
		printf("\n\n");
	}
	
}