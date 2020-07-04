#include<stdio.h>

int main(void)
{
	
	//function declarations
	int AddIntegers(int, int);
	int SubtractIntegers(int, int);
	float AddFloats(float, float);

	//variable declarations
	typedef int (*AddIntsFnPtr)(int, int);
	AddIntsFnPtr ptrAddTwoIntegers = NULL;
	AddIntsFnPtr ptrFunction = NULL;

	typedef float (*AddFloatsFnPtr)(float, float);
	AddFloatsFnPtr ptrAddTwoFloats = NULL;

	int iAnswer = 0;
	float fAnswer = 0.0f;

	//code
	ptrAddTwoIntegers = AddIntegers;
	iAnswer = ptrAddTwoIntegers(5, 25);
	printf("\n\n");
	printf("\tSum of integers = %d\n\n", iAnswer);

	ptrFunction = SubtractIntegers;
	iAnswer = ptrFunction(2, 34);
	printf("\n\n");
	printf("\tSum of floating point numbers = %f\n\n", fAnswer);

	return (0);
}

int AddIntegers(int iNum1, int iNum2)
{
	//variable declarations
	int iSum;

	//code
	iSum = iNum1 + iNum2;
	return(iSum);
}

int SubtractIntegers(int iNum1, int iNum2)
{
	//variable declarations
	int iAns;

	//code
	if(iNum1 > iNum2)
	{
		iAns = iNum1 - iNum2;
	}
	else
	{
		iAns = iNum2 - iNum1;
	}
	return(iAns);
}

float AddFloats(float fNum1, float fNum2)
{
	//variable declarations
	float iAns;

	//code
	iAns = fNum1 + fNum2;
	return (iAns);
}