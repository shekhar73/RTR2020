#include <stdio.h>
// Entry-Point Function => main() => Valid Return Type (int) and 3 Parameters (int	argc, char *argv[], char *envp[])
int main(int argc, char *argv[], char *envp[])
{
	//function prototype / declaration / signature
	void function_addition(void);

	//code
	function_addition(); //function call
	return(0);
}

// ****** USER DEFINED FUNCTION : METHOD OF DEFINITION 1 ******
// ****** NO RETURN VALUE, NO PARAMETERS ******

void function_addition(void) //function definition
{
	//variable declarations : local variables to function_addition()
	int i_a, i_b, isum;

	//code
	printf("\n\n");
	printf("Enter Integer Value For 'A' : ");
	scanf("%d", &i_a);

	printf("\n\n");
	printf("Enter Integer Value For 'B' : ");
	scanf("%d", &i_b);

	isum = i_a + i_b;
	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", i_a, i_b, isum);
}