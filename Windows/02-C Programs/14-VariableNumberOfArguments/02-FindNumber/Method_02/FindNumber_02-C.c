#include <stdio.h>
#include <stdarg.h>

#define NUMBER_TO_BE_FOUND 3
#define NUMBER_OF_ELEMNTES 10

int main(void)
{
	//function protoptypes
	void FindNumber(int, int, ...);

	//code
	printf("\n\n");

	FindNumber(NUMBER_TO_BE_FOUND, NUMBER_OF_ELEMNTES, 1, 4, 6, 2, 4, 5, 6, 4, 2, 1);

	return (0);
}


void FindNumber(int numberToBeFound, int iNumber, ...)
{
	
	int va_FindNumber(int, int, va_list);

	int iCount = 0;
	int iNum;
	va_list numberList;

	va_start(numberList, iNumber);

	iCount = va_FindNumber(numberToBeFound, iNumber, numberList);

	if(iCount == 0)
	{
		printf("\tNumber %d could not be found!!!\n\n", numberToBeFound);
	}
	else
	{
		printf("\tNumber %d found %d times !!!\n\n", numberToBeFound, iCount);
	}

	va_end(numberList);
}

int va_FindNumber(int numberToBeFound, int iNumber, va_list list)
{

	int countNumber = 0;
	int iNum;

	while(iNumber)
	{
		iNum = va_arg(list, int);
		if(iNum == numberToBeFound)
			countNumber++;
		iNumber--;
	}

	return (countNumber);
}