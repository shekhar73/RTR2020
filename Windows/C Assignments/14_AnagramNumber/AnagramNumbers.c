/*
Find if the given number us Anagram or not.
	eg: number 1 : 1234
	      number 2 : 3241
	      The given number is anagram.
	anagram: the numbers present in first array should be also present in the second array , it can be in sequence or without any sequence.		
*/

#include<stdio.h>

#include<stdlib.h>


void IsAnagram(int, int);

int main(void)
{
	int iNum1, iNum2 = 0;

	printf("Enter a number : ");
	scanf("%d", &iNum1);

	printf("\nEnter Second Number : ");
	scanf("%d", &iNum2);


	IsAnagram(iNum1, iNum2);
	return (0);
}

void IsAnagram(int iNum1, int iNum2)
{
	int iDigit = 0, Arr[9] = { 0 }, i = 0;
	int IsAnagram = 0;

	while(iNum1 != 0)
	{
		iDigit = iNum1 % 10;
		Arr[iDigit]++;
		iNum1 = iNum1 / 10;
	}

	while(iNum2 != 0)
	{
		iDigit = iNum2 % 10;
		Arr[iDigit]--;
		iNum2 = iNum2 / 10;
	}

	for(i = 0; i <= 9; i++)
	{
		if(Arr[i] != 0)
		{
		    IsAnagram = 1;
		}
		else
		{
			IsAnagram = 0;
		}
	}
	
    if(IsAnagram == 1)
	{
		printf("The given number is not an Anagram Number!\n");
	}
	else
	{
		printf("The given nubmer is an Anagram Number!\n");
	}
}
