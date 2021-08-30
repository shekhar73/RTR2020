#include <stdio.h>

int main(void)
{
	int iNumber = 0;
	float fNumber = 0.0;
	char chVal;
	
	
	
	printf("\nEnter an Integer value : ");
	scanf("%d", &iNumber);
	
	printf("\nEnter a floating point number : ");
	scanf("%f", &fNumber);
	
	printf("\nEnter a character value : ");
	scanf(" %c", &chVal);
	
	printf("\n");
	
	printf("%c\n", chVal);
	printf("%d\n", iNumber);
	printf("%f\n", fNumber);

	return (0);
}

