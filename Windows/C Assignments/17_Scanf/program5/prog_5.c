#include <stdio.h>

int main(void)
{
	int iNumber = 0;
	float fNumber = 0.0;
	char chValue;

	printf("\nEnter a character, integer and floting point value : ");
	scanf("%c %d %f", &chValue, &iNumber, &fNumber);
	
	printf("\n");

	printf("%c\n", chValue);
	printf("%d\n", iNumber);
	printf("%f\n", fNumber);
	
}

