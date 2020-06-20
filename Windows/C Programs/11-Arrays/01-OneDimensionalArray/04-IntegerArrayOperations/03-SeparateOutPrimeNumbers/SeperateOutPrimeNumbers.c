#include <stdio.h>
#define NUMBER_OF_ELEMENTS 10

int main(void)
{
	//variable declarations
	int int_array[NUMBER_OF_ELEMENTS];
	int i, inum, j, icnt = 0;

	//code
	printf("\n\n");
	
	printf("Enter Integer Elememts for Array : \n\n");
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		scanf("%d", &inum);
		
		// If 'inum' is negative ( < 0 ), then convert it to positive (multiply by -1)
		if (inum < 0)
			inum = -1 * inum;
		
		int_array[i] = inum;
	}
	
	printf("\n\n");
	printf("Array Elements Are : \n\n");
	for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		printf("%d\n", int_array[i]);
	}
	
	// *** SEPARATING OUT EVEN NUMBERS FROM ARRAY ELEMENTS ***
	printf("\n\n");
	printf("Prime Numbers amongst the array elements are : \n\n");
	/*for (i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		for (j = 1; j <= int_array[i]; j++)
		{
			if ((int_array[i] % j) == 0)
				icnt++;
		}
	
		// NUMBER 1 IS NEITHER A PRIME NUMBER NOR A CONSONANT
		// IF A NUMBER IS PRIME, IT IS ONLY DIVISIBLE BY 1 AND ITSELF.
		// HENCE, IF A NUMBER IS PRIME, THE VALUE OF 'icnt' WILL BE EXACTLY 2.
		// IF THE VALUE OF 'icnt' IS GREATER THAN 2, THE NUMBER IS DIVISIBLE BY NUMBERS OTHER THAN 1 AND ITSLEF AND HENCE, IT IS NOT PRIME
		// THE VALUE OF 'icnt' WILL BE 1 ONLY IF int_array[i] IS 1.

		if (icnt == 2)
			printf("%d\n", int_array[i]);
		
		icnt = 0; //RESET 'icnt' TO 0 FOR CHECKING THE NEXT NUMBER...
	}*/
	
	for(i = 0; i < NUMBER_OF_ELEMENTS; i++)
	{
		for(j = 2; j <= (int_array[i] / 2); j++)
		{
			if(int_array[i] % j == 0)
			{
				break;
			}
		}
		if(j == (int_array[i] / 2) + 1)
		{
			printf("%d\t", int_array[i]);
		}
	}
	
	
	return(0);
}