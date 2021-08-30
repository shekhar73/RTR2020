#include <stdio.h>
// DEFINING STRUCT
struct MyData
{
	int iNum;
	float fNum;
	double dNum;
	char ch;
};
struct MyData data = { 2, 2.2f, 3.61998, 'S' };

int main(void)
{
	//code
	//Displaying Values Of The Data Members Of 'struct MyData'
	printf("\n\n");
	printf("Data members of 'struct MyData' are : \n\n");
	printf("iNum = %d\n", data.iNum);
	printf("fNum = %f\n", data.fNum);
	printf("dNum = %lf\n", data.dNum);
	printf("ch = %c\n\n", data.ch);
	return(0);
}