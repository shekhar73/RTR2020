#include <stdio.h>

//Defining struct

struct MyData
{
	int iNum;
	float fNum;
	double dNum;
};

struct MyData data; //Declaring a single struct variable of type 'struct MyData' globally ...

int main(void)
{
	//variable declarations
	int isize;
	int fsize;
	int dsize;
	int struct_MyData_size;
	
	//code
	//Assigning Data Values To The Data Members Of 'struct MyData'
	data.iNum = 23;
	data.fNum = 23.32f;
	data.dNum = 34.34523;
	
	//Displaying Values Of The Data Members Of 'struct MyData'
	printf("\n\n");
	printf("\tData Members of struct MyData are : \n\n");
	printf("\tiNum = %d\n", data.iNum);
	printf("\tfNum = %f\n", data.fNum);
	printf("\tdNum = %lf\n", data.dNum);
	
	//Displaying Sizes (In Bytes) Of The Data Members Of 'struct MyData'
	isize = sizeof(data.iNum);
	fsize = sizeof(data.fNum);
	dsize = sizeof(data.dNum);
	
	//Printing sizes of of Data members of struct MyData
	printf("\n\n");
	printf("\tSize of Data Members of struct MyData are : \n\n");
	printf("\tSize of iNum = %d bytes\n", isize);
	printf("\tSize of fNum = %d bytes\n", fsize);
	printf("\tSize of dNum = %d bytes\n", dsize);
	
	//Calculatins size of the entire struct MyData
	struct_MyData_size = sizeof(struct MyData);
	
	printf("\n\n\tThe size of struct MyData = %d bytes\n\n", struct_MyData_size);
	return (0);
}