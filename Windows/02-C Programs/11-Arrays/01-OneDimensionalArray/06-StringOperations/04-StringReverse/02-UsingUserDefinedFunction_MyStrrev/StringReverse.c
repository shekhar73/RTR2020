//String lenght by using your own function

#include <stdio.h>

#define MAXIMUM_STRINGH_LENGTH 512

int main(void)
{
	//function prototype
	void MyStringReverese(char[], char[]);
	
	//variable declarations
	char chArray[MAXIMUM_STRINGH_LENGTH], chReversedArray[MAXIMUM_STRINGH_LENGTH];
	
	//code
	
	//String Input
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tEnter a string which you want to reverse : ");
	gets_s(chArray, MAXIMUM_STRINGH_LENGTH);
	
	//String Reverese
	MyStringReverese(chReversedArray, chArray);
	
	//String Output
	printf("\n\n");
	printf("\tThe original string entered by you is : ");
	printf("%s", chArray);
	
	printf("\n\n");
	printf("\tThe reversed string is : ");
	printf("%s", chReversedArray);

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	
	return (0);
}

void MyStringReverese(char str_destination[], char str_source[])
{
	//function prototype
	int MyStringLength(char[]);
	
	//vairable declarations
	int iStringLength = 0;
	int icnt, jcnt, lenght;
	
	//code
	iStringLength = MyStringLength(str_source);
		
	for(icnt = 0, jcnt = iStringLength - 1; icnt < jcnt, jcnt >= 0; icnt++, jcnt--)
	{
		str_destination[icnt] = str_source[jcnt];
	}
	str_destination[icnt] = '\0';
}

int MyStringLength(char str[])
{
	int i = 0, icnt = 0;
	
	while(str[i] != '\0')
	{
		icnt++;
		i++;
	}
	return (icnt);
}