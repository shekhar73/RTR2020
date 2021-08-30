#include <stdio.h>
// *** GLOBAL SCOPE ***

int main(void)
{
	//function prototypes
	void my_change_count_function(void);
	
	//variable declarations
	extern int my_global_count;
	
	//code
	printf("\n");
	printf("Value Of my_global_count before my_change_count_function() = %d\n", my_global_count);
	my_change_count_function();
	
	printf("Value Of my_global_count after my_change_count_function() = %d\n", my_global_count);
	printf("\n");
	
	return(0);
}

// *** GLOBAL SCOPE ***
// my_global_count is a global variable.
// Since, it is declared before my_change_count_function(), it can be accssed and used as any ordinary global variable in my_change_count_function()
// Since, it is declared after main(), it must be first re-declared in main() as an external global variable by means of the 
//'extern' keyword and the type of the variable.
// Once this is done, it can be used as an ordinary global variable in main as well.

int my_global_count = 0;
void my_change_count_function(void)
{
	//code
	my_global_count = 5;
	printf("Value Of my_global_count in my_change_count_function() = %d\n", my_global_count);
}
