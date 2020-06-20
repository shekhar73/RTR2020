//Find the lenth of the given string

#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//variable declarations
	
	char chArray[MAXIMUM_STRING_LENGTH];
	int iLengthOfString = 0;
	
	//code
	
	//***********STRING INPUT*************//
	printf("\n\n");
	
	printf("\tEnter a string : ");
	scanf("%[^\n]", &chArray);
	
	//**********STRING OUTPUT************//
	printf("\n");
	printf("\tThe string entered by you is : ");
	printf("%s", chArray);
	
	//***********STRING LENGTH***********//
	printf("\n");
	iLengthOfString = strlen(chArray);
	printf("\tLength of string is = %d characters !!!\n", iLengthOfString);
	
	return (0);
}
