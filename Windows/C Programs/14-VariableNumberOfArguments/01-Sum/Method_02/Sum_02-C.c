#include <stdio.h>
#include <stdarg.h>

int main(void)
{
	//function prototypes
	int CalculateSum(int, ...);

	//variable declarations
	int iAnswer;

	//code
	printf("\n\n");

	iAnswer = CalculateSum(5, 10, 20, 30, 40, 50);
	printf("\tAnswer = %d\n\n", iAnswer);

	iAnswer = CalculateSum(10, 1, 1, 1, 2, 3, 4, 5, 6, 7, 10);
	printf("\tAnswer = %d\n", iAnswer);

	iAnswer = CalculateSum(0);
	printf("\tAnswer = %d\n", iAnswer);

	return (0);
}

int CalculateSum(int num, ...)
{
	int va_CalculateSum(int, va_list);
	int iSum = 0;
	va_list numberList;

	va_start(numberList, num);

	iSum = va_CalculateSum(num, numberList);

	va_end(numberList);
	return(iSum);
}
int va_CalculateSum(int iNo1, va_list iList)
{
	int sumTotal = 0;
	int iNum;

	while(iNo1)
	{
		iNum = va_arg(iList, int);
		sumTotal = sumTotal + iNum;
		iNo1--;
	}
	
	return(sumTotal);
}