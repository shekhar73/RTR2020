#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStringLength(char[]);
	void MyStringCopy(char[], char[]);
	
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH], chArraySpcesRemoved[MAXIMUM_STRING_LENGTH];
	int iStringLength;
	int icnt = 0, jcnt = 0;
	
	//code
	//string input
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	iStringLength = MyStringLength(chArray);
	
	for(icnt = 0; icnt < iStringLength; icnt++)
	{
		if(chArray[icnt] == ' ')
		{
			continue;
		}
		else
		{
			chArraySpcesRemoved[jcnt] = chArray[icnt];
			jcnt++;
		}
	}
	
	chArraySpcesRemoved[jcnt] = '\0';
	
	//String output
	printf("\n\n");
	printf("\tString entered by you is : ");
	printf("%s\n", chArray);
	
	printf("\n\n");
	printf("\tString after removal of spaces is : ");
	printf("%s\n", chArraySpcesRemoved);

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	return (0);
}


int MyStringLength(char str[])
{
	//variable declarations
	int i = 0;
	int icnt = 0;
	
	//code
	while(str[i] != '\0')
	{
		icnt++;
		i++;
	}
	return (icnt);
}