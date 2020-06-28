#include <stdio.h>

int main(void)
{
	//variable declarations
	char charArray[10];
	char *charPtr = NULL;
	int icnt;
	
	//code
	for(icnt = 0; icnt < 10; icnt++)
	{
		charArray[icnt] = (char)(icnt + 65);
	}
	
	charPtr = charArray;
	printf("\n\n");
	printf("\tElements of the character array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t charArray[%d] = %c\n", icnt, *(charPtr + icnt));
	}
	
	printf("\n\n");
	printf("\tElements of the double array : \n\n");
	for(icnt = 0; icnt < 10; icnt++)
	{
		printf("\t charArray[%d] = %c\t\t address = %p\n", icnt, *(charPtr + icnt), (charPtr + icnt));
	}
	
	printf("\n\n");
	
	return (0);
}