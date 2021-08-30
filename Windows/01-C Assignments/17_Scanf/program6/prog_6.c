#include <stdio.h>

int main(void)
{
	int iNumber = 0;
	float fNumber = 0.0f;
	char chValue = 0;
	double dNumber = 0.0;
	long int lNumber = 0;
	long double ldNumber = 0.00;

	printf("\nEnter a space seperated Character, Integer, Float, Double, Long, Long Double values : ");
	scanf("%c %d %f %lf %ld %LE", &chValue, &iNumber, &fNumber, &dNumber, &lNumber, &ldNumber);
	
	printf("\n");

	printf("Character = %c\n", chValue);
	printf("Integer = %d\n", iNumber);
	printf("Float = %f\n", fNumber);
	printf("Double = %lf\n", dNumber);
	printf("Long = %ld\n", lNumber);
	printf("Long Double = %LE\n", ldNumber);
	
	printf("\n");
	
	return (0);
}

