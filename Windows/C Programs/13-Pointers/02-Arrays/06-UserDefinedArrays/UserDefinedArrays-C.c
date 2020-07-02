#include <stdio.h>
#include <stdlib.h>

#define INT_SIZE sizeof(int)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define CHAR_SIZE sizeof(char)

int main(void)
{
	//variable declarations
	int *iPtrArray = NULL;
	unsigned int intArrayLength = 0;
	
	float *fPtrArray = NULL;
	unsigned int floatArrayLength = 0;
	
	double *dPtrArray = NULL;
	unsigned int doubleArrayLength = 0;
	
	char *chPtrArray = NULL;
	unsigned int charArrayLength = 0;
	
	int icnt = 0;
	
	//code
	
	//***********************Integer Array*****************************
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tEnter the number of elements you want in the integer array : ");
	scanf("%u", &intArrayLength);
	
	iPtrArray = (int *)malloc(INT_SIZE * intArrayLength);
	if(iPtrArray == NULL)
	{
		printf("\n\n");
		printf("\tMemory allocation for integer array has failed !!! Exiting now...\n\n");
		exit (0);
	}
	else
	{
		printf("\n\n");
		printf("\tMemory allocation for integer array has succeeded !!!\n\n");
	}
	
	printf("\n\n");
	
	printf("\tEnter %d elements for the integer array : \n\n", intArrayLength);
	for(icnt = 0; icnt < intArrayLength; icnt++)
	{
		scanf("%d", (iPtrArray + icnt));
	}
	
	//*******************************Float Array*********************************
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tEnter the number of elements you want in the float array : ");
	scanf("%u", &floatArrayLength);
	
	fPtrArray = (float *)malloc(FLOAT_SIZE * floatArrayLength);
	if(fPtrArray == NULL)
	{
		printf("\n\n");
		printf("\tMemory allocation for float array has failed !!! Exiting now...\n\n");
		exit (0);
	}
	else
	{
		printf("\n\n");
		printf("\tMemory allocation for float array has succeeded !!!\n\n");
	}
	
	printf("\n\n");
	
	printf("\tEnter %d elements for the float array : \n\n", floatArrayLength);
	for(icnt = 0; icnt < floatArrayLength; icnt++)
	{
		scanf("%f", (fPtrArray + icnt));
	}
	
	//*******************************Double Array*********************************
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tEnter the number of elements you want in the double array : ");
	scanf("%u", &doubleArrayLength);
	
	dPtrArray = (double *)malloc(DOUBLE_SIZE * doubleArrayLength);
	if(dPtrArray == NULL)
	{
		printf("\n\n");
		printf("\tMemory allocation for double array has failed !!! Exiting now...\n\n");
		exit (0);
	}
	else
	{
		printf("\n\n");
		printf("\tMemory allocation for double array has succeeded !!!\n\n");
	}
	
	printf("\n\n");
	
	printf("\tEnter %d elements for the double array : \n\n", doubleArrayLength);
	for(icnt = 0; icnt < doubleArrayLength; icnt++)
	{
		scanf("%lf", (dPtrArray + icnt));
	}
	
	
//*******************************Char Array*********************************
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tEnter the number of elements you want in the character array : ");
	scanf("%u", &charArrayLength);
	
	chPtrArray = (char *)malloc(CHAR_SIZE * charArrayLength);
	if(chPtrArray == NULL)
	{
		printf("\n\n");
		printf("\tMemory allocation for char array has failed !!! Exiting now...\n\n");
		exit (0);
	}
	else
	{
		printf("\n\n");
		printf("\tMemory allocation for char array has succeeded !!!\n\n");
	}
	
	printf("\n\n");
	
	printf("\tEnter %d elements for the char array : \n\n", charArrayLength);
	for(icnt = 0; icnt < charArrayLength; icnt++)
	{
		*(chPtrArray + icnt) = getch();
		printf("%c\n", *(chPtrArray + icnt));
	}

	//Display of arrays
	
	//Integer array
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tThe integer array entered by you and consisting of %d Elements is as follows : \n\n", intArrayLength);
	for(icnt = 0; icnt < intArrayLength; icnt++)
	{
		printf("\t%d \t\t at address : %p\n", *(iPtrArray + icnt), (iPtrArray + icnt));
	}
	
	//Float array
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tThe float array entered by you and consisting of %d Elements is as follows : \n\n", intArrayLength);
	for(icnt = 0; icnt < floatArrayLength; icnt++)
	{
		printf("\t%f \t\t at address : %p\n", *(fPtrArray + icnt), (fPtrArray + icnt));
	}
	
	//Double array
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tThe double array entered by you and consisting of %d Elements is as follows : \n\n", intArrayLength);
	for(icnt = 0; icnt < doubleArrayLength; icnt++)
	{
		printf("\t %lf\t\t at address : %p\n", *(dPtrArray + icnt), (dPtrArray + icnt));
	}
	
	//Char array
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tThe character array entered by you and consisting of %d Elements is as follows : \n\n", intArrayLength);
	for(icnt = 0; icnt < charArrayLength; icnt++)
	{
		printf("\t %c\t\t at address : %p\n", *(chPtrArray + icnt), (chPtrArray + icnt));
	}
	
	//Freeing memory occupied by arrays (in reverse order of allocation)
	if(chPtrArray)
	{
		free(chPtrArray);
		chPtrArray = NULL;
		
		printf("\n\n");
		printf("\tMemory occupied by character array has been successfully freed !!!\n\n");
	}
	
	if(dPtrArray)
	{
		free(dPtrArray);
		dPtrArray = NULL;
		
		printf("\n\n");
		printf("\tMemory occupied by double array has been successfully freed !!!\n\n");
	}
	
	if(fPtrArray)
	{
		free(fPtrArray);
		fPtrArray = NULL;
		
		printf("\n\n");
		printf("\tMemory occupied by float array has been successfully freed !!!\n\n");
	}
	
	if(iPtrArray)
	{
		free(iPtrArray);
		iPtrArray = NULL;
		
		printf("\n\n");
		printf("\tMemory occupied by integer array has been successfully freed !!!\n\n");
	}
	
	return (0);
}