#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStringLength(char[]);
	
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH];
	int iStringLength;
	int icnt = 0, iWordCount = 0, iSpaceCount = 0;
	
	//code
	
	//String input
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	iStringLength = MyStringLength(chArray);
	
	for(icnt = 0; icnt < iStringLength; icnt++)
	{
		switch(chArray[icnt])
		{
			case 32 : //32 is the ASCII value for ' '
				iSpaceCount++;
				break;
			
			default :
				break;
		}
	}
	
	iWordCount = iSpaceCount + 1;
	
	//String output
	printf("\n\n");
	printf("\tString entered by you is : ");
	printf("%s\n", chArray);
	
	printf("\n\n");
	printf("\tNumber os spaces in the input string = %d\n\n", iSpaceCount);
	printf("\tNumber of words in the input string = %d\n\n", iWordCount);

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	return (0);
}

int MyStringLength(char str[])
{
	//variable declarations
	int i = 0;
	int icnt = 0;
	
	//code
	while(str[i] != '\0')
	{
		icnt++;
		i++;
	}
	return (icnt);
}