#include <stdio.h>

//Defining struct
struct MyData
{
	int iNum;
	float fNum;
	double dNum;
};

int main(void)
{
	//Variable declrations
	int iSize;
	int fSize;
	int dSize;
	int structMyDataSize;
	int pointerToStructMyDataSize;

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

	//Assigning data values to the data members of struct MyData
	(*pData).iNum = 33;
	(*pData).fNum = 34.45f;
	(*pData).dNum = 4.34587897;

	//Displaying values of the data members of strut MyData
	printf("\n\n");
	printf("\tData members of struct MyData are : \n\n");
	printf("\tiNum = %d\n", (*pData).iNum);
	printf("\tfNum = %f\n", (*pData).fNum);
	printf("\tdNum = %lf\n", (*pData).dNum);

	//Calculating sizes (in Bytes) of the Data members of struct MyData
	iSize = sizeof((*pData).iNum);
	fSize = sizeof((*pData).fNum);
	dSize = sizeof((*pData).dNum);
	//Displaying sizes (in Bytes) of the data members of struct MyData
	printf("\n\n");
	printf("\tSizes of Data members of struct MyData are : \n");
	printf("\tSize of iNum = %d bytes\n", iSize);
	printf("\tSize of fNum = %d bytes\n", fSize);
	printf("\tSize of dNum = %d bytes\n", dSize);

	//Calculating size of the entire struct MyData
	structMyDataSize = sizeof(struct MyData);
	pointerToStructMyDataSize = sizeof(struct MyData*);

	//Displaying sizes of the entire struct MyData
	printf("\n\n");
	printf("\tSize of struct MyData : %d bytes\n\n", structMyDataSize);
	printf("\tSize of pointer to struct MyData : %d bytes\n\n", pointerToStructMyDataSize);

	if(pData)
	{
		free(pData);
		pData = NULL;
		printf("\tMemory allocated to struct MyData has been freed tSuccessfully!!)\n");
	}

	return (0);
}