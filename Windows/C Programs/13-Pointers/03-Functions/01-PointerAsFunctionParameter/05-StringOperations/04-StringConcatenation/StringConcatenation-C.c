#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function declarations
	void MyStringConcatenation(char *, char *);
	int MyStringLengthFunction(char *);
	
	//variable declarations
	char *chArrayOne = NULL, *chArrayTwo = NULL;
	int intStringLength = 0;
	
	//code
	printf("\t\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");;
	chArrayOne = (char *)malloc(MAXIMUM_STRING_LENGTH * sizeof(char));
	if(chArrayOne == NULL)
	{
		printf("\tMemory allocation to character array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	//String input
	printf("\tEnter a first string : ");
	gets_s(chArrayOne, MAXIMUM_STRING_LENGTH);
	
	chArrayTwo = (char*)malloc(MAXIMUM_STRING_LENGTH * sizeof(char));
	if(chArrayTwo == NULL)
	{
		printf("\tMemory allocation to character array failed!!! exiting now...\n\n");
		exit(0);
	}
	
	
	printf("\tEnter a second string : ");
	gets_s(chArrayTwo, MAXIMUM_STRING_LENGTH);
	
	//String concat
	printf("\n\n");
	printf("\t****************Before Concatenation***************");
	printf("\n\n");
	printf("\tThe original first string entered by you is : ");
	printf("\t%s\n", chArrayOne);
	
	printf("\n\n");
	printf("\tThe original second string entered by you is : ");
	printf("\t%s\n", chArrayTwo);
	
	MyStringConcatenation(chArrayOne, chArrayTwo);
	printf("\n\n");
	printf("\t****************After Concatenation***************\n\n");
	printf("\tchArrayOne[] Is : ");
	printf("\t%s\n", chArrayOne);
	printf("\n\n");
	printf("\tchArrayTwo[] Is : ");
	printf("\t%s\n", chArrayTwo);
	if(chArrayTwo)
	{
		free(chArrayTwo);
		chArrayTwo = NULL;
	}
	if(chArrayOne)
	{
		free(chArrayOne);
		chArrayOne = NULL;
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

void MyStringConcatenation(char *strDestination, char *strSource)
{
	int MyStringLengthFunction(char *);
	
	//variable declarations
	int iStringLengthSource = 0, iStringLengthDestination = 0;
	
	int icnt = 0, jcnt = 0;
	
	iStringLengthSource = MyStringLengthFunction(strSource);
	iStringLengthDestination = MyStringLengthFunction(strDestination);
	
	
	for(icnt = iStringLengthDestination, jcnt = 0; jcnt < iStringLengthSource; icnt++, jcnt++)
	{
		*(strDestination + icnt) = *(strSource + jcnt);
	}
	*(strDestination + icnt) = '\0';
}