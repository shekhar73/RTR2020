//Concatenation of string by using user defined function

#include<stdio.h>

#define MAXIMUM_STRING_LENGTH 512

int main(void)
{
	//function prototype
	void MyStringConcat(char[], char[]);
	
	char chArrayFirst[MAXIMUM_STRING_LENGTH], chArraySecond[MAXIMUM_STRING_LENGTH];

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
	
	MyStringConcat(chArraySecond, chArrayFirst);
	
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

void MyStringConcat(char str_src[], char str_destination[])
{
	//function prototype
	int MyStringLength(char[]);
	
	//variable declarations
	int iLengthOfStringSource = 0, iLengthOfStringDestination = 0;
	int i, j;
	
	iLengthOfStringSource = MyStringLength(str_src);
	iLengthOfStringDestination = MyStringLength(str_destination);
	
	//code somshekhar karle
	//somshekharkarle
	for(i = iLengthOfStringDestination, j = 0; j < iLengthOfStringSource; i++, j++)
	{
		str_destination[i] = str_src[j];
		//printf("\t%c", str_src[i]);
		//printf("n-----------------------\n");
		//printf("\t%c", str_destination[j]);
		//printf("n-----------------------\n");
	}
	
	
	str_destination[i] = '\0';
	
}

int MyStringLength(char str[])
{
	//variable declarations
	int i = 0, icnt = 0;
	
	while(str[i] != '\0')
	{
		icnt++;
		i++;
	}
	return (icnt);
}