#include <stdio.h>

int main(void)
{
	//variable declrations
	int iNum;
	int *ptr = NULL;
	
	//code
	iNum = 10;
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("*************************Before ptr = &iNum ********************************\n\n");
	
	printf("\tValue of 'iNum'			 = %d\n", iNum);
	printf("\tAddress of 'iNum' 		 = %p\n", &iNum);
	printf("\tValue at address of 'iNum' = %d\n", *(&iNum));
	
	ptr = &iNum;
	
	printf("\n\n");
	
	printf("*********************After ptr = &num*********************\n");
	printf("\tValue of 'iNum' 			 = %d\n", iNum);
	printf("\tAddress of 'iNum' 		 = %p\n", ptr);
	printf("\tValue at address of 'iNum' = %d\n", *ptr);
	
	return (0);
}