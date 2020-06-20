//Find the length of string by using library function

#include<stdio.h>
#include<string.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH];
	
	//code
	//Input string
	printf("\n\n");
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	//String output
	printf("\n\n");
	printf("\tThe original string entered by you is : ");
	printf("%s", chArray);
	
	printf("\n\n");
	printf("\tThe reveresed string is : ");
	printf("%s\n", strrev(chArray));
	
	return (0);
}