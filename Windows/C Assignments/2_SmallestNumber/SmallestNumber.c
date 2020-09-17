//Find the smallest number in the given array.

#include <stdio.h>
#include <stdlib.h>

int SmallestNumber(int*, int);

int main()
{
	int iNum, i = 0, iAns = 0;
	int* ptr = NULL;

	printf("\tEnter number of elements in the array: \n");
	scanf("%d", &iNum);

	ptr = (int*)malloc(iNum * sizeof(int));

	printf("\tEnter elements in the array : \n");
	for (i = 0; i < iNum; i++)
	{
		scanf("%d", &ptr[i]);
	}

	printf("\n\n");
	printf("\tThe given array entered by your is : ");
	for (i = 0; i < iNum; i++)
	{
		printf("\t%d", ptr[i]);
	}


	iAns = SmallestNumber(ptr, iNum);

	printf("\n\n");

	printf("\tThe SMALLEST NUMBER from the given array is : %d", iAns);

	return (0);
}

int SmallestNumber(int* Arr, int iSize)
{
	//23, 43, 11, 3

	int iMin = 0, i = 0;

	iMin = Arr[i];

	for (i = 0; i < iSize; i++)
	{
		if (Arr[i] < iMin)
		{
			iMin = Arr[i];
		}
	}

	return iMin;
}