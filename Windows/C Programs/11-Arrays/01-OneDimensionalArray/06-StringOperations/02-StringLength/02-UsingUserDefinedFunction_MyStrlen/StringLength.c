//Find the lenth of string without using predefined functions

#include<stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int UserDefinedStringLength(char []);
	
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH];
	int iLengthOfString = 0;
	
	//code
	
	//**********STRING INPUT************//
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tEnter a string : ");
	scanf("%[^'\n']", &chArray);
	
	//*********STRING OUTPUT***********//
	printf("\n\n");
	printf("\tThe string Entered by you is : ");
	printf("%s", chArray);
	
	//************STRING LENGTH*********//
	printf("\n\n");
	iLengthOfString = UserDefinedStringLength(chArray);
	printf("\tLength of the given string = %d characters !!!\n", iLengthOfString);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	return (0);
}

int UserDefinedStringLength(char str[])
{
	//variable declarations
	//int i = 0;
	int icnt = 0;
	
	//code
	//Iterate the loop until you will get the \0
	
	//Using while loop
	/*while(str[i] != '\0')
	{
		icnt++;
		i++;
	}*/
	
	//Using for loop
	/*for(i = 0; i < MAXIMUM_STRING_LENGTH; i++)
	{
		if('\0' == str[i])
			break;
		else
			icnt++;
	}*/
	
	//Using for loop and without using if condition
	for(int i = 0; str[i] != '\0'; i++)
	{
		icnt++;
	}
	printf("%d", i);
	printf("\n");
	return (icnt);
}