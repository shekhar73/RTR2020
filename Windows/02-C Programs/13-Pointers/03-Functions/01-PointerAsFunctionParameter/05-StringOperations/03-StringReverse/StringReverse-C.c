#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function declarations
	void MyStringReverseFunction(char *, char *);
	int MyStringLengthFunction(char *);
	
	//variable declarations
	char *chArrayOrigianl = NULL, *chArrayReversed = NULL;
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
	chArrayReversed = (char *)malloc(intStringLength * sizeof(char));
	if(chArrayReversed == NULL)
	{
		printf("\tMemory allocation to character array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	//String copy
	MyStringReverseFunction(chArrayReversed, chArrayOrigianl);
	
	//string output
	printf("\n\n");
	printf("\tThe original string entered by you is : ");
	printf("\t%s\n", chArrayOrigianl);
	
	printf("\n\n");
	printf("\tThe reversed string is : ");
	printf("\t%s\n", chArrayReversed);
	
	if(chArrayReversed)
	{
		free(chArrayReversed);
		chArrayReversed = NULL;
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

void MyStringReverseFunction(char *strDestination, char *strSource)
{
	int MyStringLengthFunction(char *);
	
	//variable declarations
	int iStringLength = 0;
	int icnt = 0, jcnt = 0, iLength = 0;
	
	iStringLength = MyStringLengthFunction(strSource);
	iLength = iStringLength - 1;
	
	for(icnt = 0, jcnt = iLength; icnt < iStringLength, jcnt >= 0; icnt++, jcnt--)
	{
		*(strDestination + icnt) = *(strSource + jcnt);
	}
	*(strDestination + icnt) = '\0';
}