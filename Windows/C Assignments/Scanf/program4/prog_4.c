#include <stdio.h>

int main(void)
{
	int iNumber = 0;
	float fNumber = 0.0f;
	char charValue = 0;
	double dNumber = 0.0;
	long int lNumber = 0;
	long double ldNumber = 0.00;

	printf("\nEnter a charachter value : ");
	scanf("%c", &charValue);
	
	printf("\nEnter an integer number : ");
	scanf("%d", &iNumber);

	printf("\n\nCharacter value = %c\n", charValue);
	printf("\nInterger number = %d\n", iNumber);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\nEnter a floating point value : ");
	scanf("%f", &fNumber);
	
	printf("\nEnter a double number : ");
	scanf("%lf", &dNumber);
	
	printf("\nFloating point value = %f\n", fNumber);
	printf("\nDouble number = %lf\n", dNumber);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\nEnter a long number : ");
	scanf("%ld", &lNumber);
	
	printf("\nEnter a lond double number : ");
	scanf("%LE", &ldNumber);
	
	printf("\nLong number = %ld\n", lNumber);
	printf("\nLong double number = %LE\n", ldNumber);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

