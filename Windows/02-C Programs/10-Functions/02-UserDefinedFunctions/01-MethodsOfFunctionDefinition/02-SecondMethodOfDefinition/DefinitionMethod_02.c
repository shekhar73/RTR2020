#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
// Entry-Point Function => main() => Valid Return Type (int) and 3 Parameters (int argc, char *argv[], char *envp[])
int main(int argc, char *argv[], char *envp[])
{
	//function prototype / declaration / signature

	int function_addintion(void);

	//variable declarations : local variables to main()

	int iresult;

	//code

	iresult = function_addintion(); //function call
	printf("\n\n");
	printf("Sum = %d\n\n", iresult);
	return(0);
}

// ****** USER DEFINED FUNCTION : METHOD OF DEFINITION 2 ******
// ****** VALID (int) RETURN VALUE, NO PARAMETERS ******

int function_addintion(void) //function definition
{
	//variable declarations : local variables to function_addintion()
	int ia, ib, isum;
	//code
	printf("\n\n");
	
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &ia);
	
	printf("\n\n");
	
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &ib);
	
	isum = ia + ib;
	return(isum);
}