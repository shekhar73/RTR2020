#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH]; //Sting is a nothing but character array
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	//scanf("%[^\n]s", &chArray);
	//scanf("%[^\n]c", &chArray);
	//scanf("%c[^\n]", &chArray); //prints only first character
	//gets(chArray);
	//gets(chArray, MAXIMUM_STRING_LENGTH);
	//gets_s(chArray, MAXIMUM_STRING_LENGTH);
	scanf("%s[^\n]", &chArray); // prints first word i.e. if entered : "somsehkhar karle" it will print only "somshekhar"
	printf("\n\n");
	printf("\t The string entered by you is : ");
	printf("%s\n", chArray);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return (0);
}