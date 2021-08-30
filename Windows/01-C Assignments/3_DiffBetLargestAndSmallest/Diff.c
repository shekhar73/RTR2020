//Find the difference between the largest and the smallest number in the given array.

#include <stdio.h>
#include <stdlib.h>

int LargestNumber(int*, int);
int SmallestNumber(int*, int);

int main()
{
	int iNum, i = 0, iMax = 0, iMin = 0, iAns = 0;
	int* ptr = NULL;

	printf("Enter number of elements in the array: \n");
	scanf("%d", &iNum);

	ptr = (int*)malloc(iNum * sizeof(int));

	printf("Enter elements in the array : \n");
	for (i = 0; i < iNum; i++)
	{
		scanf("%d", &ptr[i]);
	}

	printf("\n\n");
	printf("The given array entered by your is : ");
	for (i = 0; i < iNum; i++)
	{
		printf("\t%d", ptr[i]);
	}


	iMax = LargestNumber(ptr, iNum);

	printf("\n\n");
	printf("The LARGEST NUMBER from the given array is : %d", iMax);


	iMin = SmallestNumber(ptr, iNum);

	printf("\n\n");
	printf("The SMALLEST NUMBER from the given array is : %d", iMin);


	iAns = iMax - iMin;

	printf("\n\n");

	printf("The Difference between Largest and Smallest Number from the given array is : %d", iAns);

	return (0);
}

int LargestNumber(int* Arr, int iSize)
{
	//23, 43, 11, 3

	int iMax = 0, i = 0;

	for (i = 0; i < iSize; i++)
	{
		if (Arr[i] > iMax)
		{
			iMax = Arr[i];
		}
	}
	
	return iMax;
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