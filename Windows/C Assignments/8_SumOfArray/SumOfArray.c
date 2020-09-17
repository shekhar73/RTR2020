//Find the sum of the total array.

#include<stdio.h>
#include<stdlib.h>

#define MALLOC(type, size) (type*)malloc(size * sizeof(type))

void SumOfArray(int*, int);

int main(void)
{
	int* ptr = NULL;
	int iNum = 0, i = 0;

	printf("Enter number of elements in the array : \n");
	scanf("%d", &iNum);

	ptr = MALLOC(int, iNum);

	printf("Enter the elements in the array : \n");
	for (i = 0; i < iNum; i++)
	{
		scanf("%d", &ptr[i]);
	}

	printf("The elements in the array are : \n");
	for (i = 0; i < iNum; i++)
	{
		printf("%d\t", ptr[i]);
	}

	SumOfArray(ptr, iNum);
	return (0);
}

void SumOfArray(int* Arr, int iSize)
{
	int i = 0, iSum = 0;

	
	for (i = 0; i < iSize; i++)
	{
		iSum += Arr[i];
	}

	printf("\nThe SUM of all the elements from the array is : %d", iSum);
}

