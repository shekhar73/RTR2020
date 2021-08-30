#include <stdio.h>

#define INT_ARRAY_SIZE 10
#define FLOAT_ARRAY_SIZE 5
#define CHAR_ARRAY_SIZE 26

#define NUMBER_OF_STRINGS 10
#define MAXIMUM_NUMBER_CHARACTERS_PER_STRING 20

#define ALPHABET_BEGINING 65 //'A'

struct MyDataFirst
{
	int iArray[INT_ARRAY_SIZE];
	float fArray[FLOAT_ARRAY_SIZE];
};

struct MyDataTwo
{
	char cArray[CHAR_ARRAY_SIZE];
	char strArray[NUMBER_OF_STRINGS][MAXIMUM_NUMBER_CHARACTERS_PER_STRING];
};

int main(void)
{
	//variable declarations
	struct MyDataFirst data1;
	struct MyDataTwo data2;
	int icnt;
	
	//code
	//Piece meal assignment
	data1.fArray[0] = 1.1f;
	data1.fArray[1] = 2.2f;
	data1.fArray[2] = 3.3f;
	data1.fArray[3] = 4.4f;
	data1.fArray[4] = 5.5f;
	
	//Loop assignment
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter %d integers : \n\n", INT_ARRAY_SIZE);
	for(icnt = 0; icnt < INT_ARRAY_SIZE; icnt++)
	{
		scanf("\t%d", &data1.iArray[icnt]);
	}
	
	for(icnt = 0; icnt < CHAR_ARRAY_SIZE; icnt++)
	{
		data2.cArray[icnt] = (char)(icnt + ALPHABET_BEGINING);
	}
	
	//Piece meall assignment
	strcpy(data2.strArray[0], "Welcome!!!");
	strcpy(data2.strArray[1], "This");
	strcpy(data2.strArray[2], "is");
	strcpy(data2.strArray[3], "ASTROMEDICOMP");
	strcpy(data2.strArray[4], "Real");
	strcpy(data2.strArray[5], "Time");
	strcpy(data2.strArray[6], "Rendering");
	strcpy(data2.strArray[7], "Batch");
	strcpy(data2.strArray[8], "of");
	strcpy(data2.strArray[9], "2020-2021");
	
	//Displaying data members of struct Data1
	printf("\n\n");
	printf("\tMembers of struct Data1 alongwith their assigned values are : ");
	
	printf("\n\n");
	printf("\tInteger array (data1.iArray[]) : \n\n");
	for(icnt = 0; icnt < INT_ARRAY_SIZE; icnt++)
		printf("\tdata1.iArray[%d] = %d\n", icnt, data1.iArray[icnt]);
	
	printf("\n\n");
	printf("\tFloating point array (data1.fArray[]) : \n\n");
	for(icnt = 0; icnt < FLOAT_ARRAY_SIZE; icnt++)
		printf("\tdata1.fArray[%d] = %f\n", icnt, data1.fArray[icnt]);
	
	//Displaying data members of struct Data2
	printf("\n\n");
	printf("\tMembers of struct Data2 alongwith their assigned values are : ");
	
	printf("\n\n");
	printf("\tCharacter Array array (data2.cArray[]) : \n\n");
	for(icnt = 0; icnt < CHAR_ARRAY_SIZE; icnt++)
		printf("\tdata2.cArray[%d] = %c\n", icnt, data2.cArray[icnt]);
	
	printf("\n\n");
	printf("\tString Array (data2.strArray[]) : \n\n");
	for (icnt = 0; icnt < NUMBER_OF_STRINGS; icnt++)
		printf("%s ", data2.strArray[icnt]);

	printf("\n\n");
	return (0);
}