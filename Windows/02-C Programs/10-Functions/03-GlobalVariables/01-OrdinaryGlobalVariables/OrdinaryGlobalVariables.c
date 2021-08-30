#include <stdio.h>
// *** GLOBAL SCOPE ***
//If not initialized by us, global variables are initialized to their zero values
//(with respect to their data types i.e: 0 for int, 0.0 for float and double, etc.) by default.
//But still, for good programming discipline, we shall explicitly initialize our global variable with 0.

int my_global_count = 0;
int main(void)
{
	//function prototypes
	void my_change_count_one_function(void);
	void my_change_count_two_function(void);
	void my_change_count_three_function(void);
	
	//code
	printf("\n");
	printf("main() : Value of my_global_count = %d\n", my_global_count);
	my_change_count_one_function();
	my_change_count_two_function();
	my_change_count_three_function();
	printf("\n");
	return(0);
}

// *** GLOBAL SCOPE ***
void my_change_count_one_function(void)
{
	//code
	my_global_count = 100;
	printf("my_change_count_one_function() : Value of my_global_count = %d\n", my_global_count);
}

// *** GLOBAL SCOPE ***
void my_change_count_two_function(void)
{
	//code
	my_global_count = my_global_count + 1;
	printf("my_change_count_two_function() : Value of my_global_count = %d\n", my_global_count);
}

// *** GLOBAL SCOPE ***
void my_change_count_three_function(void)
{
	//code
	my_global_count = my_global_count + 10;
	printf("my_change_count_three_function() : Value of my_global_count = %d\n", my_global_count);
}
// *** GLOBAL SCOPE ***