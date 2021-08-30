#include <stdio.h>
int main(void)
{
	//variable declarations
	float f_num;
	float f_number = 1.7f; //simply change this value ONLY to get different outputs...
	
	//code
	printf("\n\n");
	printf("Printing Numbers %f to %f : \n\n", f_num, (f_number * 10.0f));

	f_num = f_number;

	while(f_num <= (f_number * 10.0f))
	{
		printf("\t%f\n", f_num);
		f_num = f_number + f_number;
	}
	printf("\n\n");
	return(0);
}