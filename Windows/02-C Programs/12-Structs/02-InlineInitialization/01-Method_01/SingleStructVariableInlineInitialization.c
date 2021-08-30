#include <stdio.h>
// DEFINING STRUCT
struct MyData
{
	int iNum;
	float fNum;
	double dNum;
	char ch;
} data = { 23, 4.4f, 21.45241995, 'S' }; //Inline initialization of struct variable 'data' of type 'struct MyData'

int main(void)
{
	//code
	//Displaying Values Of The Data Members Of 'struct MyData'
	printf("\n\n");
	printf("\tData members of 'struct MyData' are : \n\n");
	printf("\tiNum = %d\n", data.iNum);
	printf("\tfNum = %f\n", data.fNum);
	printf("\tdNum = %lf\n", data.dNum);
	printf("\tch = %c\n\n", data.ch);
	return(0);
}