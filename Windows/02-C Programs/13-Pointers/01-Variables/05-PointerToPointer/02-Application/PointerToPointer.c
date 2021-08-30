#include <stdio.h>

int main(void)
{
	//variable declarations
	int iNum;
	int *iPtr = NULL;
	int **pPtr = NULL;
	
	//code
	iNum = 10;
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\t************************Before iPtr = &iNum*******************************\n");
	printf("\tValue of iNum				= %d\n", iNum);
	printf("\tAddress of iNum			= %p\n", &iNum);
	printf("\tValue at Address of iNum	= %d\n", *(&iNum));
	
	iPtr = &iNum;
	
	printf("\n\n");
	
	
	printf("\t************************After iPtr = &iNum*******************************\n");
	printf("\tValue of iNum				= %d\n", iNum);
	printf("\tAddress of iNum			= %p\n", iPtr);
	printf("\tValue at Address of iNum	= %d\n", *iPtr);
	
	pPtr = &iPtr;
	
	printf("\n\n");
	printf("\t************************After pPtr = &iPtr*******************************\n");
	
	printf("\tValue of iNum							= %d\n", iNum);
	printf("\tAddress of iNum (iPtr)				= %p\n", iPtr);
	printf("\tAddress of iPtr (pPtr)				= %p\n", pPtr);
	printf("\tValue at Address of iPtr (*pPtr)		= %p\n", *pPtr);
	printf("\tValue at Address of (*iPtr) (*pPtr)	= %d\n", **pPtr);
	
	return (0);
}