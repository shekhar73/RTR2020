#include <stdio.h>

int main(void)
{
	int iNumber = 0;
	float fNumber = 0.0;
	char chVal;
	double dNumer = 0.0;
	long int lNumber = 0;

	printf("\nEnter a character value : ");
	scanf("%c", &chVal);
	
	printf("\nEnter an Interger number : ");
	scanf("%d", &iNumber);
	
	printf("\nEnter float number : ");
	scanf("%f", &fNumber);
	
	printf("\nEnter a double number : ");
	scanf("%lf", &dNumer);
	
	printf("\nEnter a long number : ");
	scanf("%ld", &lNumber);
	
	printf("\n");

	printf("%c\n", chVal);
	printf("%d\n", iNumber);
	printf("%f\n", fNumber);
	printf("%lf\n", dNumer);
	printf("%ld\n", lNumber);
	
	return (0);
}

