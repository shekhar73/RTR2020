#include <stdio.h>
#include <ctype.h>  //for toupper()

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	
	int MyStringLength(char[]);
	
	//variable declarations
	char chArray[MAXIMUM_STRING_LENGTH], chArrayUpperCaseFirstLetter[MAXIMUM_STRING_LENGTH];
	int iStringLength;
	int i, j;
	
	//code
	
	//Input string
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter a string : ");
	gets_s(chArray, MAXIMUM_STRING_LENGTH);
	
	iStringLength = MyStringLength(chArray);
	j = 0;
	
	for(i = 0; i < iStringLength; i++)
	{
		if(i == 0)
			chArrayUpperCaseFirstLetter[j] = toupper(chArray[i]);
		else if(chArray[i] == ' ')
		{
			chArrayUpperCaseFirstLetter[j] = chArray[i];
			chArrayUpperCaseFirstLetter[j + 1] = toupper(chArray[i + 1]);
			i++;
			j++;
		}
		else
		{
			chArrayUpperCaseFirstLetter[j] = chArray[i];
		}
		j++;
	}
	
	chArrayUpperCaseFirstLetter[j] = '\0';
	
	//string output
	printf("\n\n");
	printf("\tString entered by your is : ");
	printf("%s\n", chArray);
	
	printf("\n\n");
	printf("\tString after capitalizing first letter of every word : ");
	printf("%s\n", chArrayUpperCaseFirstLetter);
	
	
	return(0);
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