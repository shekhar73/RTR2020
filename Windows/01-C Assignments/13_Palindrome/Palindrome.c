// Find whether the given array is palindrome or not.

#include<stdio.h>

#include<stdlib.h>

#define MALLOC(type, size) (type*)malloc(size * sizeof(type))

void IsPalindrome(int*, int);

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

	IsPalindrome(ptr, iNum);
	return (0);
}

void IsPalindrome(int *Arr, int iSize)
{
	int i = 0, j = iSize - 1, icnt = 0;

	while(i <= j)
	{
		if(Arr[i] != Arr[j])
		{
			icnt = 1;
			break;
		}
		i++;
		j--;
	}

	if(icnt == 0)
	{
		printf("\nThe given array is a palindrome array !!!:)");
	}
	else
	{
	    printf("\nThe given array is not a palindrome array!!!");
	}
}
