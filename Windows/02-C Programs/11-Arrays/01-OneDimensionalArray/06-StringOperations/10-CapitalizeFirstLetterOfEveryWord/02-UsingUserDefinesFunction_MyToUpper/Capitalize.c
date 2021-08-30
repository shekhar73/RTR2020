#include <stdio.h>

#define MAXIMUM_STRING_LENGTH 512

#define SK_SPACE ' '
#define SK_FULLSTOP '.'
#define SK_COMMA ','
#define SK_EXCLAMATION '!'
#define SK_QUESTION_MARK '?'

int main(void)
{
	//function prototype
	
	int MyStringLength(char[]);
	char MyToUpperLetter(char);
	
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
			chArrayUpperCaseFirstLetter[j] = MyToUpperLetter(chArray[i]);
		else if(chArray[i] == SK_SPACE)
		{
			chArrayUpperCaseFirstLetter[j] = chArray[i];
			chArrayUpperCaseFirstLetter[j + 1] = MyToUpperLetter(chArray[i + 1]);
			i++;
			j++;
		}
		else if((chArray[i] == SK_FULLSTOP || chArray[i] == SK_COMMA || chArray[i] == SK_EXCLAMATION
				|| chArray[i] == SK_QUESTION_MARK) && (chArray[i] != SK_SPACE))
	
		{
			chArrayUpperCaseFirstLetter[j] = chArray[i];
			chArrayUpperCaseFirstLetter[j + 1] = SK_SPACE;
			chArrayUpperCaseFirstLetter[j + 2] = MyToUpperLetter(chArray[i + 1]);
			j = j + 2;
			i++;
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

char MyToUpperLetter(char ch)
{
	int iNumber = 0, c = 0;
	
	iNumber = 'a' - 'A';
	
	if((int)ch >= 97 && (int)ch <= 122)
	{
		c = (int)ch - iNumber;
		return ((char)c);
	}
	else
	{
		return(ch);
	}
}