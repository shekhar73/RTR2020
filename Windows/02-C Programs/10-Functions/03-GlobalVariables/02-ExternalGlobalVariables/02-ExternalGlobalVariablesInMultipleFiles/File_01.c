// *** GLOBAL SCOPE ***
// astro_global_count is a global variable declared in source code file ExternalGlobalVariablesInMultipleFiles.c
// To access it in this file, it must first be re-declared as an external variable
// in the global scope of this file along with the 'extern' keyword and its proper data type
// Then, it can be used as any ordinary global variable throughout this file as well

#include <stdio.h>
extern int astro_global_count;
void astro_change_count_one(void)
{
	//code
	astro_global_count = astro_global_count + 1;
	printf("change_count_one() : Value Of astro_global_count in File_01 = %d\n", astro_global_count);
}