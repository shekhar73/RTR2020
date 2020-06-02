#include <stdio.h>

int main(void)
{
	//function prototypes
	void PrintBinaryFormofNumber(unsigned int);

	//vaiable decalrations
	unsigned int sk_ia;

	//code
	printf("\n\n");
	printf("Enter first Number = ");
	scanf("%u", &sk_ia);

	
	PrintBinaryFormofNumber(sk_ia);
	
	return (0);
}


void PrintBinaryFormofNumber(unsigned int sk_decimal_number)
{
	//variable declarations
	unsigned int sk_quotient;
	unsigned int sk_inum;
	long int sk_binary_array[55] = {0};
	long int sk_icnt = 0, sk_length = 0;

	
	printf("The Ocatl form of the Decimal Integer %d is\t=\t", sk_decimal_number);

	sk_inum = sk_decimal_number;
	sk_icnt = 0;

	/*while (sk_inum != 0)
	{
		sk_binary_array[sk_icnt] = sk_inum % 8;
		sk_inum = sk_inum / 8;
		sk_icnt++;
		sk_length++;
	}*/

	 unsigned long long int ocatalnum = 0, temp = 1;

	while(sk_inum != 0)
	{
		ocatalnum = ocatalnum + (sk_inum % 8) * temp;
		sk_inum = sk_inum / 8;
		temp = temp * 10;
	}

	printf("%llu\n", ocatalnum);
/*
	for (sk_icnt = sk_length - 1; sk_icnt >= 0 ; sk_icnt--)
	{
		
		printf("%ld", sk_binary_array[sk_icnt]);
			
	}*/
	
}