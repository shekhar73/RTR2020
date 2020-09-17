// Find the even number in the array and make it odd.
#include<stdio.h>
#include<stdlib.h>

#define MALLOC(type, size) (type*)malloc(size * sizeof(type))

void FindEvenAndMakeItOdd(int*, int);

int main(void)
{
	int* ptr = NULL;
	int iNum = 0, i = 0;

	printf("Enter number of elements in the array : \n");
	scanf("%d", &iNum);

	//ptr = (int*)malloc(iNum * sizeof(int));

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

	FindEvenAndMakeItOdd(ptr, iNum);
	return (0);
}

void FindEvenAndMakeItOdd(int* Arr, int iSize)
{
	int i = 0, iCnt = 0;

	printf("\nThe updated odd elements array is : \n");
	for (i = 0; i < iSize; i++)
	{
		if (Arr[i] % 2 == 0)
		{
			printf("%d\t", Arr[i] + 1);
		}
	}
}

