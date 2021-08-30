#include <stdio.h>
int main(void)
{
	//variable declarations
	float fnum;
	float f_num = 1.7f; //simply change this value ONLY to get different outputs...
	
	//code
	printf("\n\n");

	printf("Printing Numbers from %f to %f : \n\n", f_num, (f_num * 10.0f));
	fnum = f_num;
	do
	{
		printf("\t%f\n", fnum);
		fnum = fnum + f_num;
	}while (fnum <= (f_num * 10.0f));
	printf("\n\n");
	return(0);
}