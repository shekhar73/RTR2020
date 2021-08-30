#include <stdio.h>

//Difining struct

struct MyData
{
	int iNumber;
	float fNumber;
	double dNumber;
	char ch;
}mdata; //Declaring a single struct varibale of type 'struct MyData' globally

int main(void)
{
	// variable declarations
	
	int int_size;
	int float_size;
	int double_size;
	int char_size;
	int struct_MyData_size;
	
	//code
	//Assigning Data types values to the data members of 'struct MyData'
	mdata.iNumber = 30;
	mdata.fNumber = 23.45f;
	mdata.dNumber = 2.34345;
	mdata.ch = 'S';
	
	//Displaying values of the data members of 'struct MyData'
	printf("\n\n");
	printf("\tData members of 'struct MyData' are : \n");
	printf("\tiNumber = %d\n", mdata.iNumber);
	printf("\tfNumber = %f\n", mdata.fNumber);
	printf("\tdNumber = %lf\n", mdata.dNumber);
	printf("\tch = %c\n", mdata.ch);
	
	//Calculating sizes (In Bytes) of the data members of 'struct MyData'
	int_size = sizeof(mdata.iNumber);
	float_size = sizeof(mdata.fNumber);
	double_size = sizeof(mdata.dNumber);
	char_size = sizeof(mdata.ch);
	
	
	//Displaying sizes (in bytes) of the data members of 'struct 'MyData'
	printf("\n\n");
	printf("\tSizes (in bytes) of DATA MEMBERS of 'Struct MyData' are : \n");
	printf("\tSize of iNumber = %d bytes\n", int_size);
	printf("\tSize of fNumber = %d bytes\n", float_size);
	printf("\tSize of dNumber = %d bytes\n", double_size);
	printf("\tSize of char_ch = %d bytes\n", char_size);
	
	//calcluating size (in bytes) of the entire struct MyData
	
	struct_MyData_size = sizeof(struct MyData); //Can also give stuct name -> sizeof(MyData)
	
	//Displaying size of the struct MyData
	printf("\n\n");
	printf("\tSize of struct MyData = %d bytes\n\n", struct_MyData_size);
	
	
	return (0);
}