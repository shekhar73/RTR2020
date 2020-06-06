#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
#include <ctype.h> //'ctype.h' contains declaration of 'atoi()'
#include <stdlib.h> // 'stdlib.h' contains declaration of 'exit()'
int main(int argc, char *argv[], char *envp[])
{
	//variable declarations
	int sk_i;
	int sk_num;
	int sk_sum = 0;
	//code
	if(argc == 1)
	{
		printf("\n\n");
		printf("No Numbers Given For Addition !!! Exiting now ...\n\n");
		printf("Usage : CommandLineArgumentsApplication <first number> <second number> ...\n\n");
		exit(0);
	}
	// *** THIS PROGRAMS ADDS ALL COMMAND LINE ARGUMENTS GIVEN IN INTEGER FORM ONLY AND OUTPUTS THE SUM ***
	// *** DUE TO USE OF atoi(), ALL COMMAND LINE ARGUMENTS OF TYPES OTHER THAN 'int' ARE IGNORED ***
	
	printf("\n\n");
	
	printf("Sum Of All Integer Command Line Arguments Is : \n\n");
	for (sk_i = 1; sk_i < argc; sk_i++) //loop starts from i = 1 because, i = 0 will result in 'argv[i]' = 'argv[0]' which is the name of the program itself i.e : 'CommandLineArgumentsApplication.exe'
	{
		sk_num = atoi(argv[sk_i]);
		sk_sum = sk_sum + sk_num;
	}
	printf("Sum = %d\n\n", sk_sum);
	return(0);
}