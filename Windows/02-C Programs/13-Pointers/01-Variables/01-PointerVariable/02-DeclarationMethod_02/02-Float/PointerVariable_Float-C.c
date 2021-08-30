#include <stdio.h>

int main(void)
{
	//variable declrations
	float iNum;
	float* ptr = NULL;
	
	//code
	iNum = 73.77f;
	
	printf("\t*************************Before ptr = &iNum ********************************\n\n");
	
	printf("\tValue of 'iNum'			 = %f\n", iNum);
	printf("\tAddress of 'iNum' 		 = %p\n", &iNum);
	printf("\tValue at address of 'iNum' = %f\n", *(&iNum));
	
	ptr = &iNum;
	
	printf("\n\n");
	
	printf("*********************After ptr = &num*********************\n");
	printf("\tValue of 'iNum' 			 = %f\n", iNum);
	printf("\tAddress of 'iNum' 		 = %p\n", ptr);
	printf("\tValue at address of 'iNum' = %f\n", *ptr);
	
	return (0);
}