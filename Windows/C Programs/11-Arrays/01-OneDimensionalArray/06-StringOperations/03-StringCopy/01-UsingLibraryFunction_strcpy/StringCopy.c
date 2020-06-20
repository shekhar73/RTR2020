#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//variable declarations
	
	char chArrayMain[MAXIMUM_STRING_LENGTH], chArrayCopy[MAXIMUM_STRING_LENGTH];
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArrayMain, MAXIMUM_STRING_LENGTH);
	
	strcpy(chArrayCopy, chArrayMain);
	
	printf("\n\n");
	printf("\tThe original string entered by you (i.e. chArrayMain[]) is : ");
	printf("%s\n", chArrayMain);
	
	printf("\n\n");
	printf("\tThe copied string (i.e. chArrayCopy[]) is : ");
	printf("\t%s\n", chArrayCopy);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return (0);
}