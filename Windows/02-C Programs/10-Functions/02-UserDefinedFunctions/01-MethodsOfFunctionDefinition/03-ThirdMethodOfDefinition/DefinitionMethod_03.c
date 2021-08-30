#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
// Entry-Point Function => main() => Valid Return Type (int) and 3 Parameters (int argc, char *argv[], char *envp[])
int main(int argc, char *argv[], char *envp[])
{
	//function prototype / declaration / signature
	void function_addition(int, int);

	//variable declarations : local variables to main()
	int int_a, int_b;
	//code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &int_a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &int_b);

	function_addition(int_a, int_b); //function call
	return(0);
}

// ****** USER DEFINED FUNCTION : METHOD OF DEFINITION 3 ******
// ****** NO RETURN VALUE, VALID PARAMETERS (int, int) ******

void function_addition(int int_a, int int_b) //function definition
{
	//variable declarations : local variables to function_addition()
	int int_sum;
	//code
	int_sum = int_a + int_b;
	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", int_a, int_b, 	int_sum);
}