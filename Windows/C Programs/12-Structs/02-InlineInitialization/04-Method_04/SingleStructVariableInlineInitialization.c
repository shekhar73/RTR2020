#include <stdio.h>

struct MyData
{
	int iNum;
	float fNum;
	double dNum;
	char ch;
};

int main(void)
{
	struct MyData data_one = { 32, 3.93f, 3.243765, 'S'};
	
	struct MyData data_two = { 'S', 26.2f, 43.199523, 99 };
	
	struct MyData data_three = { 73, 'S' };
	
	struct MyData data_four = { 100 };
	
	//code
	//Displaying Values Of The Data Members Of 'struct MyData'
	printf("\n\n");
	printf("Data members 'struct MyData data_one' are : \n\n");
	printf("\tiNum = %d\n", data_one.iNum);
	printf("\tfNum = %f\n", data_one.fNum);
	printf("\tdNum = %lf\n", data_one.dNum);
	printf("\tch = %c\n", data_one.ch);
	
	printf("\n\n");
	printf("\tData members of 'struct MyData data_two' are : \n\n");
	printf("\tiNum = %d\n", data_two.iNum);
	printf("\tfNum = %f\n", data_two.fNum);
	printf("\tdNum = %lf\n", data_two.dNum);
	printf("\tch = %c\n", data_two.ch);
	
	printf("\n\n");
	printf("Data members of 'struct MyData data_three' are : \n\n");
	printf("\tiNum = %d\n", data_three.iNum);
	printf("\tfNum = %f\n", data_three.fNum);
	printf("\tdNum = %lf\n", data_three.dNum);
	printf("\tch = %c\n", data_three.ch);
	printf("\n\n");
	
	printf("Data members of 'struct MyData data_four' are : \n\n");
	printf("\tiNum = %d\n", data_four.iNum);
	printf("\tfNum = %f\n", data_four.fNum);
	printf("\tdNum = %lf\n", data_four.dNum);
	printf("\tch = %c\n", data_four.ch);
	
	return(0);
}