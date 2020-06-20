//String concatenation by using library function

#include <stdio.h>
#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//variable declarations
	char chArrayFirst[MAXIMUM_STRING_LENGTH], chArraySecond[MAXIMUM_STRING_LENGTH];
	
	//code
	
	//Sting Input
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a first string : ");
	//scanf("%[^\n]", &chArrayFirst);
	gets_s(chArrayFirst, MAXIMUM_STRING_LENGTH);
	
	printf("\tEnter a second string : ");
	//scanf("%[^\n]", &chArraySecond);
	gets_s(chArraySecond, MAXIMUM_STRING_LENGTH);
	
	//String Concatenation
	printf("\n\n");
	printf("\t**************************************THE STRING BEFORE CONCATENATION*********************************");
	printf("\n\n");
	printf("\tThe original first string entered by you is : ");
	printf("%s", chArrayFirst);

	printf("\n\n");
	printf("\tThe original second string entered by you is : ");
	printf("%s", chArraySecond);
	
	strcat(chArrayFirst, chArraySecond);
	
	printf("\n\n");
	printf("\t**************************************THE STRING AFTER CONCATENATION*********************************");
	printf("\n\n");
	printf("\t'chArrayFirst[]' is : ");
	printf("%s", chArrayFirst);
	
	printf("\n\n");
	printf("\t'chArraySecond[]' is : ");
	printf("%s", chArraySecond);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return (0);
}