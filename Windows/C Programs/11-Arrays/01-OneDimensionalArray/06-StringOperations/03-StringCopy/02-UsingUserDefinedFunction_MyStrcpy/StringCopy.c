#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	void MyStringCopy(char[], char[]);
	
	//variable declarations
	char chArrayMain[MAXIMUM_STRING_LENGTH], chArrayCopy[MAXIMUM_STRING_LENGTH];
	
	//code
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArrayMain, MAXIMUM_STRING_LENGTH);
	
	MyStringCopy(chArrayCopy, chArrayMain);
	
	printf("\n\n");
	printf("\tThe original string entered by you is : ");
	printf("%s\n", chArrayMain);
	
	printf("\n\n");
	printf("\tThe copied string is : ");
	printf("%s\n\n", chArrayCopy);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	return (0);
}

void MyStringCopy(char str_destination[], char str_source[])
{
	//function prototype
	int MyStringLength(char[]);
	
	//variable declarations
	int iStringLength = 0;
	int j;
	
	//code
	iStringLength = MyStringLength(str_source);
	for(j = 0; j < iStringLength; j++)
	{
		str_destination[j] = str_source[j];
	}
	str_destination[j] = '\0';
	
}

int MyStringLength(char str[])
{
	//variable declarations
	int j = 0;
	int icnt = 0;
	
	while(str[j] != '\0')
	{
		icnt++;
		j++;
	}
	return (icnt);
}