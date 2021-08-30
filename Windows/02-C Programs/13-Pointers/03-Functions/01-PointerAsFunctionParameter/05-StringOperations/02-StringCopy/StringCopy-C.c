#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_STRING_LENGTH 512



int main(void)
{
	//function declarations
	void MyStringCopyFunction(char *, char *);
	int MyStringLengthFunction(char *);
	
	//variable declarations
	char *chArrayOrigianl = NULL, *chArrayCopy = NULL;
	int intStringLength = 0;
	
	//code
	printf("\t\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");;
	chArrayOrigianl = (char *)malloc(MAXIMUM_STRING_LENGTH * sizeof(char));
	if(chArrayOrigianl == NULL)
	{
		printf("\tMemory allocation to character array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	//String input
	printf("\tEnter a string : ");
	gets_s(chArrayOrigianl, MAXIMUM_STRING_LENGTH);
	
	intStringLength = MyStringLengthFunction(chArrayOrigianl);
	chArrayCopy = (char *)malloc(intStringLength * sizeof(char));
	if(chArrayCopy == NULL)
	{
		printf("\tMemory allocation to character array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	//String copy
	MyStringCopyFunction(chArrayCopy, chArrayOrigianl);
	
	//string output
	printf("\n\n");
	printf("\tThe original string entered by you is : ");
	printf("\t%s\n", chArrayOrigianl);
	
	printf("\n\n");
	printf("\tThe copied string is : ");
	printf("\t%s\n", chArrayCopy);
	
	if(chArrayCopy)
	{
		free(chArrayCopy);
		chArrayCopy = NULL;
	}
	if(chArrayOrigianl)
	{
		free(chArrayOrigianl);
		chArrayOrigianl = NULL;
	}
	return (0);
}

int MyStringLengthFunction(char *str)
{
	//variable declarations
	int icnt = 0;
	
	//code
	while(*str != '\0')
	{
		str++;
		icnt++;
	}
	return (icnt);
}

void MyStringCopyFunction(char *strDestination, char *strSource)
{
	int MyStringLengthFunction(char *);
	//variable declarations
	int iStringLength = 0;
	int icnt = 0;
	
	iStringLength = MyStringLengthFunction(strSource);
	for(icnt = 0; icnt < iStringLength; icnt++)
	{
		*(strDestination + icnt) = *(strSource + icnt);
	}
	*(strDestination + icnt) = '\0';
}