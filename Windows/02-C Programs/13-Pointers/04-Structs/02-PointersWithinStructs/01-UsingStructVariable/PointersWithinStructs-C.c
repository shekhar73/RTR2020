#include <stdio.h>

//Defining Struct
struct MyData
{
	int *iPtr;
	int iNum;

	float *fPtr;
	float fNum;

	double *dPtr;
	double dNum;
};

int main(void)
{
	//Variable Declarations
	struct MyData data;

	//Code
	data.iNum = 23;
	data.iPtr = &data.iNum;

	data.fNum = 34.342f;
	data.fPtr = &data.fNum;

	data.dNum = 46.3444234;
	data.dPtr = &data.dNum;

	printf("\n\n");
	printf("\tiNum = %d\n", *(data.iPtr));
	printf("\tAddress of iNum = %p\n", data.iPtr);

	printf("\n\n");
	printf("\tfNum = %f\n", *(data.fPtr));
	printf("\tAddress of fNum = %p\n", data.fPtr);

	printf("\n\n");
	printf("\tdNum = %lf\n", *(data.dPtr));
	printf("\tAddress of dNum = %p\n", data.dPtr);

	return (0);
}