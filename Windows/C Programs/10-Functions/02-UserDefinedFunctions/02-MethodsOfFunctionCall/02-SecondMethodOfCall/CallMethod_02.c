#include <stdio.h> //'stdio.h' contains declaration of 'printf()'
// ****** USER DEFINED FUNCTIONS : METHOD OF CALLING FUNCTION 2 ******
// ****** CALLING ONLY TWO FUNCTIONS DIRECTLY IN main(), REST OF THE FUNCTIONS TRACE THEIR CALL INDIRECTLY TO main() ******

int main(int argc, char *argv[], char *envp[])
{
	//function prototypes
	void display_information_function(void);
	void display_country_function(void);
	
	//code
	display_information_function(); //function call
	display_country_function(); //function call
	return(0);
}

// ****** User-Defined Functions' Definitions... ***
void display_information_function(void) //function definition
{
	//function prototypes
	void my_function(void);
	void my_function_name(void);
	void is_function(void);
	void first_name_function(void);
	void fucntion_middle_name(void);
	void function_surname(void);
	void fucntion_ofRTR(void);
	
	//code
	// *** FUNCTION CALLS ***
	
	my_function();
	my_function_name();
	is_function();
	first_name_function();
	fucntion_middle_name();
	function_surname();
	fucntion_ofRTR();
}

void my_function(void) //function definition
{
	//code
	printf("\n\n");
	printf("\tMy");
}

void my_function_name(void) //function definition
{
	//code
	printf("\n\n");
	printf("\tName");
}

void is_function(void) //function definition
{
	//code
	printf("\n\n");
	printf("\tIs");
}

void first_name_function(void) //function definition
{
	printf("\n\n");
	printf("\tSomshekhar");
}

void fucntion_middle_name(void) //function definition
{
	printf("\n\n");
	printf("\tSidharam");
}

void function_surname(void) //function definition
{
	printf("\n\n");
	printf("\tKarle");
}

void fucntion_ofRTR(void) //function definition
{
	printf("\n\n");
	printf("\tStudent of RTR 2020");
}

void display_country_function(void) //function definition
{
	printf("\n\n");
	printf("\tI live In India and I love my country.");
	printf("\n\n");
}