#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
// ****** USER DEFINED FUNCTIONS : METHOD OF CALLING FUNCTION 1 ******
// ****** CALLING ALL FUNCTIONS IN main() DIRECTLY ******
// Entry-Point Function => main() => Valid Return Type (int) and 3 Parameters (int argc, char *argv[], char *envp[])

int main(int argc, char *argv[], char *envp[])
{

	//function prototypes OR declarations
	
	void my_addition_function(void);
	int my_subtraction_function(void);
	void my_multiplication_function(int, int);
	int my_division_function(int, int);

	//variable declarations
	int int_result_subtraction;
	int int_a_multiplication, int_b_multiplication;
	int int_a_division, int_b_division, int_result_division;
	
	//code
	// *** ADDITION ***
	my_addition_function(); //function call
	
	// *** SUBTRACTION ***
	int_result_subtraction = my_subtraction_function(); //function call
	printf("\n\n");
	printf("Subtraction Yields Result = %d\n", int_result_subtraction);

	// *** MULTIPLICATION ***
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Multiplication : ");
	scanf("%d", &int_a_multiplication);
	printf("\n\n");
	printf("Enter Integer Value For 'B' For Multiplication : ");
	scanf("%d", &int_b_multiplication);
	my_multiplication_function(int_a_multiplication, int_b_multiplication); //function call

	// *** DIVISION ***
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Division : ");
	scanf("%d", &int_a_division);
	printf("\n\n");
	printf("Enter Integer Value For 'B' For Division : ");
	scanf("%d", &int_b_division);
	int_result_division = my_division_function(int_a_division, int_b_division); //function call
	printf("\n\n");

	printf("Division Of %d and %d Gives = %d (Quotient)\n", int_a_division, int_b_division, int_result_division);
	printf("\n\n");
	return(0);
}

// *** Function Definition Of my_addition_function() ******
void my_addition_function(void) //function definition
{
	//variable declarations : local variables to my_addition_function()
	int ia, ib, isum;
	//code
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Addition : ");
	scanf("%d", &ia);
	printf("\n\n");
	printf("Enter Integer Value For 'B' For Addition : ");
	scanf("%d", &ib);
	isum = ia + ib;
	printf("\n\n");
	printf("Sum Of %d And %d = %d\n\n", ia, ib, isum);
}

// *** Function Definition Of my_subtraction_function() ******
int my_subtraction_function(void) //function definition
{
	//variable declarations : local variables to my_subtraction_function()
	int ia, ib, isubtraction;
	//code
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Subtraction : ");
	scanf("%d", &ia);
	printf("\n\n");
	printf("Enter Integer Value For 'B' For Subtraction : ");
	scanf("%d", &ib);
	isubtraction = ia - ib;
	return(isubtraction);
}

// *** Function Definition Of my_multiplication_function() ******
void my_multiplication_function(int int_a, int int_b) //function definition
{
	//variable declarations : local variables to my_multiplication_function()
	int int_multiplication;
	//code
	int_multiplication = int_a * int_b;
	printf("\n\n");
	printf("Multiplication Of %d And %d = %d\n\n", int_a, int_b, int_multiplication);
}

// *** Function Definition Of my_division_function() ******
int my_division_function(int int_a, int int_b) //function definition
{
	//variable declarations : local variables to my_division_function()
	int int_division_quotient;
	//code
	if (int_a > int_b)
		int_division_quotient = int_a / int_b;
	else
		int_division_quotient = int_b / int_a;
	return(int_division_quotient);
}