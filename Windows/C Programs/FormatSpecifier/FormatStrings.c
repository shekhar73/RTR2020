#include <stdio.h>
int main(void)
{
	//code
	printf("\n\n");
	printf("---------------------****************************************************-------------------------");
	printf("\n\n");

	printf("Hello World !!!\n\n");

	int sk_iNum = 45;
	printf("Integer Decimal Value Of 'sk_iNum' = %d\n", sk_iNum);
	printf("Integer Octal Value Of 'sk_iNum' = %o\n", sk_iNum);
	printf("Integer Hexadecimal Value Of 'sk_iNum' (Hexadecimal Letters In Lower Case) = %x\n", sk_iNum);
	printf("Integer Hexadecimal Value Of 'sk_iNum' (Hexadecimal Letters In Upper Case) = %X\n\n", sk_iNum);

	char s_ch = 'S';
	printf("Character s_ch = %c\n", s_ch);
	char str[] = "Somshekhar Sidharam Karle";
	printf("String str = %s\n\n", str);

	long sk_long_num = 232344564L;
	printf("Long Integer = %ld\n\n", sk_long_num);

	unsigned int sk_uib = 7;
	printf("Unsigned Integer 'sk_uib' = %u\n\n", sk_uib);

	float sk_f_num = 0703.1994;
	printf("Floating Point Number With Just %%f 'sk_f_num' = %f\n", sk_f_num);
	printf("Floating Point Number With %%4.2f 'sk_f_num' = %4.2f\n", sk_f_num);
	printf("Floating Point Number With %%6.5f 'sk_f_num' = %6.5f\n\n", sk_f_num);

	double sk_d_pi = 3.14159265358979323846;
	printf("Double Precision Floating Point Number Without Exponential = %g\n", sk_d_pi);
	printf("Double Precision Floating Point Number With Exponential (Lower Case) = %e\n", sk_d_pi);
	printf("Double Precision Floating Point Number With Exponential (Upper Case) = %E\n\n", sk_d_pi);
	printf("Double Hexadecimal Value Of 'sk_d_pi' (Hexadecimal Letters In Lower Case) = %a\n", sk_d_pi);
	printf("Double Hexadecimal Value Of 'sk_d_pi' (Hexadecimal Letters In Upper Case) = %A\n\n", sk_d_pi);

	printf("---------------------****************************************************-------------------------");
	printf("\n\n");
	return(0);
}
