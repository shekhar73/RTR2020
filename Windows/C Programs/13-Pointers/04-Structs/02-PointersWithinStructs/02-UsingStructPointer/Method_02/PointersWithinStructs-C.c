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
	struct MyData *pData = NULL;
	
	//code
	printf("\n\n");

	pData = (struct MyData *)malloc(sizeof(struct MyData));
	if(pData == NULL)
	{
		printf("\tFailed to allocate memory to struct MyData!!!\n");
		exit (0);	
	}
	else
	{
		printf("\tSuccessfully allocated memory to struct MyData!!!\n");
	}
	//Code
	pData -> iNum = 23;
	pData -> iPtr = &(pData -> iNum);

	pData -> fNum = 34.342f;
	pData -> fPtr = &(pData -> fNum);

	pData -> dNum = 46.3444234;
	pData -> dPtr = &(pData -> dNum);

	printf("\n\n");
	printf("\tiNum = %d\n", *(pData -> iPtr));
	printf("\tAddress of iNum = %p\n", pData -> iPtr);

	printf("\n\n");
	printf("\tfNum = %f\n", *(pData -> fPtr));
	printf("\tAddress of fNum = %p\n", pData -> fPtr);

	printf("\n\n");
	printf("\tdNum = %lf\n", *(pData -> dPtr));
	printf("\tAddress of dNum = %p\n", pData -> dPtr);

	if(pData)
	{
		free(pData);
		pData = NULL;
		printf("\tMemory allocated to struct MyData has been freed Successfully!!\n");
	}

	return (0);
}