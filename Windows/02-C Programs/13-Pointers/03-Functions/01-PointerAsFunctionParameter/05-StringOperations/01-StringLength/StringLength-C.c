#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function declarations
	int MyStringLengthFunction(char *);
	
	//variable declarations
	char *chArray = NULL;
	int intStringLength = 0;
	
	//code
	printf("\t\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");;
	chArray = (char *)malloc(MAXIMUM_STRING_LENGTH * sizeof(char));
	if(chArray == NULL)
	{
		printf("\tMemory allocation to character array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	//String input
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	//string output
	printf("\n\n");
	printf("\tString entered by you is : \n\n");
	printf("\t%s\n", chArray);
	
	//string length
	printf("\n\n");;
	intStringLength = MyStringLengthFunction(chArray);
	printf("\tLength of given string is = %d characters long !!!\n\n", intStringLength);
	
	if(chArray)
	{
		free(chArray);
		chArray = NULL;
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