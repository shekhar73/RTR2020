#include <stdio.h>
#include <conio.h>
struct MyData
{
	int inum;
	float fnum;
	double dnum;
	char ch;
};
int main(void)
{
	//variable declarations
	struct MyData data; //declaring a single struct variable
	//code
	//User Input For Values Of Data Members Of 'struct MyData'
	printf("\n\n");
	printf("Enter Integer Value For Data Member 'inum' Of 'struct MyData' : \n");
	scanf("%d", &data.inum);
	printf("Enter Floating-Point Value For Data Member 'fnum' Of 'struct MyData' :\n");
	scanf("%f", &data.fnum);
	printf("Enter 'Double' Value For Data Member 'dnum' Of 'struct MyData' : \n");
	scanf("%lf", &data.dnum);
	printf("Enter Character Value For Data Member 'ch' Of 'struct MyData' : \n");
	data.ch = getch();
	//Display Values Of Data Members Of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("inum = %d\n", data.inum);
	printf("fnum = %f\n", data.fnum);
	printf("dnum = %lf\n", data.dnum);
	printf("ch = %c\n\n", data.ch);
	return(0);
}