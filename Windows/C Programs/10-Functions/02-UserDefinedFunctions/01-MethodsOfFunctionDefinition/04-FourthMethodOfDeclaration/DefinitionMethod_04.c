#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
// Entry-Point Function => main() => Valid Return Type (int) and 3 Parameters (int argc, char *argv[], char *envp[])
int main(int argc, char *argv[], char *envp[])
{
	//function prototype / declaration / signature
	int my_addition_function(int, int);

	//variable declarations : local variables to main()
	int ia, ib, iresult;

	//code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &ia);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &ib);

	iresult = my_addition_function(ia, ib); //function call
	
	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", ia, ib, iresult);
	return(0);
}

// ****** USER DEFINED FUNCTION : METHOD OF DEFINITION 4 ******
// ****** VALID (int) RETURN VALUE, VALID PARAMETERS (int, int) ******

int my_addition_function(int ia, int ib) //function definition
{
	//variable declarations : local variables to my_addition_function()
	int isum;
	//code
	isum = ia + ib;
	return(isum);
}