#include <stdio.h>

int main(void)
{
	float fNumber1 = 0, fNumber2 = 0, fNumber3 = 0, fNumber4 = 0, fNumber5 = 0;
	float fAvgerage = 0.0f;

	printf("Enter space seperated 5 floating point values : ");
	scanf("%f %f %f %f %f", &fNumber1, &fNumber2, &fNumber3, &fNumber4, &fNumber5);

	fAvgerage = (fNumber1 + fNumber2 + fNumber3 + fNumber4 + fNumber5) / 5;

	printf("\nAverage of 5 floating point values is = %f \n\n", fAvgerage);
	
	return (0);
	
}

