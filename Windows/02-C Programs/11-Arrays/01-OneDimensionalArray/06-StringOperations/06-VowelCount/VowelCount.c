#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	int MyStringLength(char[]);
	
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH];
	int iStringLength;
	
	int icnt_A = 0, icnt_E = 0, icnt_I = 0, icnt_O = 0, icnt_U = 0;
	int i = 0;
	
	//code
	
	//string input
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	//string output
	printf("\n\n");
	printf("\tString Entered by you is : ");
	printf("%s\n", chArray);
	
	iStringLength = MyStringLength(chArray);
	
	for(i = 0; i < iStringLength; i++)
	{
		switch(chArray[i])
		{
			case 'A':
			case 'a':
				icnt_A++;
				break;
			
			case 'E':
			case 'e':
				icnt_E++;
				break;
			
			case 'I':
			case 'i':
				icnt_I++;
				break;
			
			case 'O':
			case 'o':
				icnt_O++;
				break;
			
			case 'U':
			case 'u':
				icnt_U++;
				break;
			
			default:
				break;
		}
	}
	
	printf("\n\n");
	printf("\tIn the string entered by you, the vowles and the number of thier occurances are as follows : \n\n");
	printf("\t'A' has occured = %d times !\n", icnt_A);
	printf("\t'E' has occured = %d times !\n", icnt_E);
	printf("\t'I' has occured = %d times !\n", icnt_I);
	printf("\t'O' has occured = %d times !\n", icnt_O);
	printf("\t'U' has occured = %d times !\n", icnt_U);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
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