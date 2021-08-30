#include <stdio.h>

int main(void)
{
	//variable declarations
	int intArray[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	int *ptrArray = NULL;
	
	//code
	printf("\n\n");
	printf("\t***Using array name as a array i.e. :  value of xth element of intArray : intArray[x] and address of xth element of intArray[x]***\n");
	printf("\tInteger array elements and their adddresses : \n\n");
	printf("\tintArray[0] = %d \t\t at address &intArray[0] : %p\n", intArray[0], &intArray[0]);
	printf("\tintArray[1] = %d \t\t at address &intArray[0] : %p\n", intArray[1], &intArray[1]);
	printf("\tintArray[2] = %d \t\t at address &intArray[0] : %p\n", intArray[2], &intArray[2]);
	printf("\tintArray[3] = %d \t\t at address &intArray[0] : %p\n", intArray[3], &intArray[3]);
	printf("\tintArray[4] = %d \t\t at address &intArray[0] : %p\n", intArray[4], &intArray[4]);
	printf("\tintArray[5] = %d \t\t at address &intArray[0] : %p\n", intArray[5], &intArray[5]);
	printf("\tintArray[6] = %d \t\t at address &intArray[0] : %p\n", intArray[6], &intArray[6]);
	printf("\tintArray[7] = %d \t\t at address &intArray[0] : %p\n", intArray[7], &intArray[7]);
	printf("\tintArray[8] = %d \t\t at address &intArray[0] : %p\n", intArray[8], &intArray[8]);
	printf("\tintArray[9] = %d \t\t at address &intArray[0] : %p\n", intArray[9], &intArray[9]);
	
	ptrArray = intArray;
	
	printf("\n\n");
	printf("\t*********Using pointer as pointer i.e. value of xth element of intArray : (ptrArray + x) and address of xth element of intArray : (ptrArray + x)*********\n\n");
	printf("\tInteger array elements and their adddresses : \n\n");

	printf("\t *(ptrArray + 0) = %d \t at address (ptrArray + 0) : %p\n", *(ptrArray + 0), (ptrArray + 0));
	printf("\t *(ptrArray + 1) = %d \t at address (ptrArray + 1) : %p\n", *(ptrArray + 1), (ptrArray + 1));
	printf("\t *(ptrArray + 2) = %d \t at address (ptrArray + 2) : %p\n", *(ptrArray + 2), (ptrArray + 2));
	printf("\t *(ptrArray + 3) = %d \t at address (ptrArray + 3) : %p\n", *(ptrArray + 3), (ptrArray + 3));
	printf("\t *(ptrArray + 4) = %d \t at address (ptrArray + 4) : %p\n", *(ptrArray + 4), (ptrArray + 4));
	printf("\t *(ptrArray + 5) = %d \t at address (ptrArray + 5) : %p\n", *(ptrArray + 5), (ptrArray + 5));
	printf("\t *(ptrArray + 6) = %d \t at address (ptrArray + 6) : %p\n", *(ptrArray + 6), (ptrArray + 6));
	printf("\t *(ptrArray + 7) = %d \t at address (ptrArray + 7) : %p\n", *(ptrArray + 7), (ptrArray + 7));
	printf("\t *(ptrArray + 8) = %d \t at address (ptrArray + 8) : %p\n", *(ptrArray + 8), (ptrArray + 8));
	printf("\t *(ptrArray + 9) = %d \t at address (ptrArray + 9) : %p\n", *(ptrArray + 9), (ptrArray + 9));
	
	return (0);
}