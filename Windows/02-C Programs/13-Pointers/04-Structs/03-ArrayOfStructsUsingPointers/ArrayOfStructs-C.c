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
	//Function prototype
	void ChangeValues(struct MyData *);

	//Variable deckarations
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
	pData -> iNum = 33;
	pData -> fNum = 34.45f;
	pData -> dNum = 4.34587897;

	//Displaying values of the data members of strut MyData
	printf("\n\n");
	printf("\tData members of struct MyData are : \n\n");
	printf("\tiNum = %d\n", pData -> iNum);
	printf("\tfNum = %f\n", pData -> fNum);
	printf("\tdNum = %lf\n", pData -> dNum);

	ChangeValues(pData);

	//Displaying values of the data members of strut MyData
	printf("\n\n");
	printf("\tData members of struct MyData are : \n\n");
	printf("\tiNum = %d\n", pData -> iNum);
	printf("\tfNum = %f\n", pData -> fNum);
	printf("\tdNum = %lf\n", pData -> dNum);

	if(pData)
	{
		free(pData);
		pData = NULL;
		printf("\tMemory allocated to struct MyData has been freed Successfully!!\n");
	}

	return (0);
}

void ChangeValues(struct MyData *pParamData)
{
	pParamData -> iNum = 23;
	pParamData -> fNum = 2.3f;
	pParamData -> dNum = 5.3454545;
}