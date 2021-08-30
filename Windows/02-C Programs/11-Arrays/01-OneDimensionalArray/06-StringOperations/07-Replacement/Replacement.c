#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStringLength(char[]);
	void MyStringCopy(char[], char[]);
	
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH], chArrayVowelReplaced[MAXIMUM_STRING_LENGTH];
	int iStringLength;
	int icnt;
	
	//code
	
	//String input
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	//string output
	MyStringCopy(chArrayVowelReplaced, chArray);
	
	iStringLength = MyStringLength(chArrayVowelReplaced);
	
	for(icnt = 0; icnt < iStringLength; icnt++)
	{
		switch(chArrayVowelReplaced[icnt])
		{
			case 'A':
			case 'a':
			
			case 'E':
			case 'e':
			
			case 'I':
			case 'i':
			
			case 'O':
			case 'o':
			
			case 'U':
			case 'u':
				chArrayVowelReplaced[icnt] = '*';
				break;
			
			default:
				break;
		}
	}
	
	//string output
	printf("\n\n");
	printf("\tString entered by you is : ");
	printf("%s\n", chArray);
	
	printf("\n\n");
	printf("\tString after replacement of vowels by * is : ");
	printf("%s\n", chArrayVowelReplaced);
	
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

void MyStringCopy(char str_destination[], char str_source[])
{
	//function prototype
	int MyStringLength(char []);
	
	//variable declarations
	int iStringLength = 0;
	int j = 0;
	
	//code
	iStringLength = MyStringLength(str_source);
	
	for(j = 0; j < iStringLength; j++)
	{
		str_destination[j] = str_source[j];
	}
	str_destination[j] = '\0';
}