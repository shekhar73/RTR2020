#include <stdio.h>
#include <stdlib.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	char* ReplaceVowelsWithHashSymbol(char *);
	
	char originalString[MAXIMUM_STRING_LENGTH];
	char *replacedString = NULL;
	
	printf("\n\n");
	printf("\tEnter a string : ");
	gets_s(originalString, MAXIMUM_STRING_LENGTH);
	
	replacedString = ReplaceVowelsWithHashSymbol(originalString);
	
	if(replacedString == NULL)
	{
		printf("\tReplaceVowelsWithHashSymbol() function has failed !!! Exitiing now...\n\n");
		exit(0);
	}
	
	printf("\n\n");
	printf("\tReplace string is : \n\n");
	printf("\t%s\n\n", replacedString);
	
	if(replacedString)
	{
		free(replacedString);
		replacedString = NULL;
	}
	return (0);
}

char* ReplaceVowelsWithHashSymbol(char *str)
{
	//function prototype
	void MyStringCopy(char *, char *);
	int MyStringLength(char *);
	
	//varibale declarations
	char *new_string = NULL;
	int icnt;
	
	//code
	new_string = (char *)malloc(MyStringLength(str) * sizeof(char));
	if (new_string == NULL)
	{
		printf("COULD not allocate memory for new string !!!\n\n");
		return(NULL);
	}
	
	MyStringCopy(new_string, str);
	for(icnt = 0; icnt < MyStringLength(new_string); icnt++)
	{
		switch(new_string[icnt])
		{
			case 'A' :
			case 'a' :
			
			case 'E' :
			case 'e' :
			
			case 'I' :
			case 'i' :
			
			case 'O' :
			case 'o' :
			
			case 'U' :
			case 'u' :
				new_string[icnt] = '#';
				break;
			default :
				break;
		}
	}
	
	return (new_string);
}

void MyStringCopy(char *strDestination, char *strSource)
{
	int MyStringLength(char *);
	//variable declarations
	int iStringLength = 0;
	int icnt = 0;
	
	iStringLength = MyStringLength(strSource);
	for(icnt = 0; icnt < iStringLength; icnt++)
	{
		*(strDestination + icnt) = *(strSource + icnt);
	}
	*(strDestination + icnt) = '\0';
}


int MyStringLength(char *str)
{
	//variable declarations
	int icnt = 0;
	
	//code
	while(*str != '\0')
	{
		str++;
		icnt++;
	}
	return (icnt);
}
