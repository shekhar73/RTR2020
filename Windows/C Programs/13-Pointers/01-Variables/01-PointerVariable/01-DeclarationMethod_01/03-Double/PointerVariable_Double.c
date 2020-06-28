#include <stdio.h>

int main(void)
{
	//variable declrations
	double iNum;
	double *ptr = NULL;
	
	//code
	iNum = 73.77345345345;
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("*************************Before ptr = &iNum ********************************\n\n");
	
	printf("\tValue of 'iNum'			 = %lf\n", iNum);
	printf("\tAddress of 'iNum' 		 = %p\n", &iNum);
	printf("\tValue at address of 'iNum' = %lf\n", *(&iNum));
	
	ptr = &iNum;
	
	printf("\n\n");
	
	printf("*********************After ptr = &num*********************\n");
	printf("\tValue of 'iNum' 			 = %lf\n", iNum);
	printf("\tAddress of 'iNum' 		 = %p\n", ptr);
	printf("\tValue at address of 'iNum' = %lf\n", *ptr);
	
	return (0);
}