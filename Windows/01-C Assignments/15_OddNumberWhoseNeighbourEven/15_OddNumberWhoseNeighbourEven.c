//In the given array print such an odd element who’s neighbour are even.
//	eg: 1 2 3 4 5 6 7 7 8 10 11 12
//	      o/p: 3 5 11	

#include <stdio.h>
#include <stdlib.h>

void PrintOdd(int*, int);

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

	PrintOdd(ptr, iNum);

	return (0);
}

void PrintOdd(int* Arr, int iSize)
{
	//eg: 1 2 3 4 5 6 7 8 9 10 11 12
	//o/p: 3 5 11	

	int icnt = 0, i = 0, j = 0;

	printf("\nOdd Number whose neighbours are an Even Numbers : \n");
	for (i = 0; i < iSize; i++)
	{
		if(i + 2 <= iSize)
		{
			if(Arr[i] % 2 == 0 && Arr[i + 2] % 2 == 0)
			{
				if(Arr[i + 1] % 2 != 0)
			   	{
			   		printf("%d\t", Arr[i + 1]);
			   	}
			}	
		}
		
	   
	}
}