// Search the elements in the array and print the prime number.

#include <stdio.h>
#include <stdlib.h>

void SearchPrime(int*, int);

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
	printf("The array entered by you is : \n");
	for (i = 0; i < iNum; i++)
	{
		printf("%d\t", ptr[i]);
	}

	SearchPrime(ptr, iNum);

	return (0);
}

void SearchPrime(int* Arr, int iSize)
{
	//23, 43, 11, 3

	int icnt = 0, i = 0, j = 0;

	printf("\nPrime numbers from the given array are : \n");
	for (i = 0; i < iSize; i++)
	{
	    icnt = 0;
		for (j = 2; j <= Arr[i] / 2; j++)
		{
			if ((Arr[i] % j) == 0)
			{
				icnt++;
				break;
			}
		}
		if (icnt == 0)
		{
			printf("%d\t", Arr[i]);
		}
	}
}