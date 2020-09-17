//Take an array of your own , swap numbers in the array.

#include<stdio.h>

#include<stdlib.h>

#define MALLOC(type, size) (type*)malloc(size * sizeof(type))

void SwapArray(int*, int);

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

	SwapArray(ptr, iNum);
	printf("The array after swapping : \n");
	for(i = 0; i < iNum; i++)
	{
		printf("%d\t", Arr[i]);
	}
	return (0);
}

void SwapArray(int *Arr, int iSize)
{
	int i = 0, j = iSize - 1;

	while(i <= j)
	{
		temp = Arr[i];
		Arr[i] = Arr[j];
		Arr[j] = temp;
		i++;
		j--;
	}
}