#include <stdio.h>

int main(void)
{
	int iNumber1 = 0, iNumber12;
	float fNumber1 = 0.0f, fNumber2 = 0.0f;
	double dNumber1 = 0.0, dNumber2 = 0.0;


	printf("\nEnter space seperated 2 integer values : ");
	scanf("%d %d", &iNumber1, &iNumber12);
	printf("->%d\n\n", iNumber1 + iNumber12);
	
	printf("\nEnter space seperated float values : ");
	scanf("%f %f", &fNumber1, &fNumber2);
	printf("->%f\n\n", fNumber1 + fNumber2);
	
	printf("\nEnter space seperated 2 double values : ");	
	scanf("%lf %lf", &dNumber1, &dNumber2);
	printf("->%lf\n\n", dNumber1 + dNumber2);
	
	return (0);
	
}

