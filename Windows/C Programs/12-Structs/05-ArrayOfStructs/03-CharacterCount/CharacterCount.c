#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXIMUM_STRING_LENGTH 1024

struct CharacterCount
{
	char ch;
	int chCount;
} characterAndCount[] = { { 'A', 0 }, 
						{ 'B', 0 }, 
						{ 'C', 0 }, 
						{ 'D', 0 }, 
						{ 'E', 0 }, 
						{ 'F', 0 },
						{ 'G', 0 },
						{ 'H', 0 },
						{ 'I', 0 },
						{ 'J', 0 },
						{ 'K', 0 },
						{ 'L', 0 },
						{ 'M', 0 },
						{ 'N', 0 },
						{ 'O', 0 },
						{ 'P', 0 },
						{ 'Q', 0 },
						{ 'R', 0 },
						{ 'S', 0 },
						{ 'T', 0 },
						{ 'U', 0 },
						{ 'V', 0 },
						{ 'W', 0 },
						{ 'X', 0 },
						{ 'Y', 0 },
						{ 'Z', 0 } };
						
#define SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS sizeof(characterAndCount)
#define SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS sizeof(characterAndCount[0])
#define NUMBER_ELEMENTS_IN_ARRAY (SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS / SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS)

int main(void)
{
	//variable declarations
	char string[MAXIMUM_STRING_LENGTH];
	int icnt, jcnt, actualStringLength = 0;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(string, MAXIMUM_STRING_LENGTH);
	
	
	actualStringLength = strlen(string);
	
	printf("\n\n");
	printf("\tThe sgting you have entered by you is : ");
	printf("%s\n\n", string);
	
	for(icnt = 0; icnt < actualStringLength; icnt++)
	{
		for(jcnt = 0; jcnt < NUMBER_ELEMENTS_IN_ARRAY; jcnt++)
		{
			string[icnt] = toupper(string[icnt]);
			
			if(string[icnt] == characterAndCount[jcnt].ch)
				characterAndCount[jcnt].chCount++;
		}
	}
	printf("\n\n");
	printf("\tThe number of occurances of All charachter from the alphabet are as follows : \n\n");
	for(icnt = 0; icnt < NUMBER_ELEMENTS_IN_ARRAY; icnt++)
	{
		printf("\tCharachter %c = %d\n", characterAndCount[icnt].ch, characterAndCount[icnt].chCount);
	}
	printf("\n\n");
	return (0);
}	
