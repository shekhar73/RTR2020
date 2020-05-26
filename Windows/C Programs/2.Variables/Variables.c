#include<stdio.h>

int main(void)
{
	//variable declarations
	int i = 5;
	float f = 3.9;
	double d = 8.04232;
	char c = 'A';

	//code
	
	printf("\n\n---------**************-----------\n\n");

	printf("i = %d\n", i);
	printf("f = %f\n", f);
	printf("d = %lf\n", d);
	printf("c = %c\n", c);

	printf("\n\n---------**************-----------\n\n");

	i = 73;
	f = 7.19f;
	d = 28.3434;
	c = 'S';

	printf("i = %d\n", i);
	printf("f = %f\n", f);
	printf("d = %lf\n", d);
	printf("c = %c\n", c);

	printf("\n\n---------**************-----------\n\n");

	return (0);
}