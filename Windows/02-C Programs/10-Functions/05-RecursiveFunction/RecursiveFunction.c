//
// RecursiveFunction.c
//

#include <stdio.h>
int main(int argc, char *argv[], char *envp[])
{
	// variable declarations
	unsigned int intnum;

	// function prototype
	void recursive_function(unsigned int);

	// code
	printf("\n\n");
	printf("Enter Any Number : \n");
	scanf("%u", &intnum);
	printf("\n\n");
	printf("Output Of Recursive Function : \n\n");
	recursive_function(intnum);
	printf("\n\n");
	return(0);
}

void recursive_function(unsigned int num)
{
	// code
	printf("num = %d\n", num);
	
	if(num > 0)
	{
		recursive_function(num - 1);
	}
}