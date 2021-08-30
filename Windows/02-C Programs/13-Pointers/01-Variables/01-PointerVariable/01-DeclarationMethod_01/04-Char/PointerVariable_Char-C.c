#include <stdio.h>

int main(void)
{
	//variable declrations
	char ch;
	char *ptr = NULL;
	
	//code
	ch = 'A';
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("*************************Before ptr = &ch ********************************\n\n");
	
	printf("\tValue of 'ch'			 = %c\n", ch);
	printf("\tAddress of 'ch' 		 = %p\n", &ch);
	printf("\tValue at address of 'ch' = %c\n", *(&ch));
	
	ptr = &ch;
	
	printf("\n\n");
	
	printf("*********************After ptr = &ch*********************\n");
	printf("\tValue of 'ch' 			 = %c\n", ch);
	printf("\tAddress of 'ch' 		 = %p\n", ptr);
	printf("\tValue at address of 'ch' = %c\n", *ptr);
	
	return (0);
}