#include <stdio.h>
int main(void)
{
	//variable declarations
	float f_num;
	float f_number = 1.1f; //simply change this value ONLY to get different outputs...
	
	//code
	printf("\n\n");
	printf("Printing Numbers %f to %f : \n\n", f_number, (f_number * 10.0f));
	for (f_num = f_number; f_num <= (f_number * 10.0f); f_num = f_num + f_number)
	{
		printf("\t%f\n", f_num);
	}
	printf("\n\n");
	return(0);
}