#include <stdio.h>
// *** GLOBAL SCOPE ***

int astro_global_count = 0;
int main(void)
{
	//function prototypes
	void astro_change_count(void);
	void astro_change_count_one(void); //function defines in File_01.c
	void astro_change_count_two(void); //function defines in File_02.c
	
	//code
	printf("\n");
	astro_change_count();
	astro_change_count_one(); //function defines in File_01.c
	astro_change_count_two(); //function defines in File_02.c
	return(0);
}

void astro_change_count(void)
{
	//code
	astro_global_count = astro_global_count + 1;
	printf("Global Count = %d\n", astro_global_count);
}