#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
// ****** USER DEFINED FUNCTIONS : METHOD OF CALLING FUNCTION 3 ******
// ****** CALLING ONLY ONE FUNCTION DIRECTLY IN main(), REST OF THE FUNCTIONS TRACE THEIR CALL INDIRECTLY TO main() ******

int main(int argc, char *argv[], char *envp[])
{
	//function prototypes
	void my_country_function();
	
	//code
	my_country_function();
	return(0);
}

void my_country_function(void) //function definition
{
	// fucntion declarations
	
	void my_RTR_fucntion(void);
	
	// code
	my_RTR_fucntion();
	printf("\n\n");
	printf("\tI live In India.");
	printf("\n\n");
}

void my_RTR_fucntion(void) //function definition
{
	// function declarations

	void my_surname_function(void);
	
	// code
	my_surname_function();
	printf("\n\n");
	printf("\tOf RTR 2020");
}

void my_surname_function(void) //function definition
{
	// function declarations
	void my_middlename_function(void);

	// code
	my_middlename_function();
	printf("\n\n");
	printf("\tKarle");
}
	
void my_middlename_function(void) //function definition
{
	// function declarations
	void my_firstname_function(void);
	
	
	// code
	my_firstname_function();
	printf("\n\n");
	printf("\tSidharam");
}

void my_firstname_function(void) //function definition
{
	// function declarations
	void is_function(void);

	// code
	is_function();
	printf("\n\n");
	printf("\tSomshekhar");
}

void is_function(void) //function definition
{
	// function declarations
	void my_function_name(void);

	//code
	my_function_name();
	printf("\n\n");
	printf("\tIs");
}
	
void my_function_name(void) //function definition
{
	// function declarations
	void my_function(void);

	//code
	my_function();
	printf("\n\n");
	printf("\tName");
}

// ****** User-Defined Functions' Definitions... ***
void my_function(void) //function definition
{
	//code
	printf("\n\n");
	printf("\tMy");
}