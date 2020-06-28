#include <stdio.h>

int main(void)
{
	//variable declarations
	int iNum;
	int *ptr = NULL;
	int iAns;
	
	//code
	iNum = 7;
	ptr = &iNum;
	
	printf("\n\n");
	printf("\t iNum		= %d\n", iNum);
	printf("\t &iNum	= %p\n", &iNum);
	printf("\t *(&iNum) = %d\n", *(&iNum));
	printf("\t ptr		= %p\n", ptr);
	printf("\t *ptr		= %d\n", *ptr);
	printf("\n\n");
	
	printf("\t Answer of (ptr + 10) = %p\n", (ptr + 10));
	
	printf("\t Answer of *(ptr + 10) = %d\n", *(ptr + 10));
	
	printf("\t Answer of (*ptr + 10) = %d\n\n", (*ptr + 10));
	
	++*ptr;
	
	printf("\t Answer of ++*ptr = %d\n", *ptr);
	
	*ptr++;
	printf("\t Answer of *ptr++ = %d\n", *ptr);
	
	ptr = &iNum;
	(*ptr)++;
	
	printf("\t Answer of (*ptr)++ = %d\n", *ptr);
	
	return (0);
}