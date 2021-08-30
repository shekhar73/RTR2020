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
	printf("*(intArray + 0) = %d \t At Address (intArray + 0) : %p\n", *(intArray + 0),(intArray + 0));
	printf("*(intArray + 1) = %d \t At Address (intArray + 1) : %p\n", *(intArray + 1),(intArray + 1));
	printf("*(intArray + 2) = %d \t At Address (intArray + 2) : %p\n", *(intArray + 2),(intArray + 2));
	printf("*(intArray + 3) = %d \t At Address (intArray + 3) : %p\n", *(intArray + 3),(intArray + 3));
	printf("*(intArray + 4) = %d \t At Address (intArray + 4) : %p\n", *(intArray + 4),(intArray + 4));
	printf("*(intArray + 5) = %d \t At Address (intArray + 5) : %p\n", *(intArray + 5),(intArray + 5));
	printf("*(intArray + 6) = %d \t At Address (intArray + 6) : %p\n", *(intArray + 6),(intArray + 6));
	printf("*(intArray + 7) = %d \t At Address (intArray + 7) : %p\n", *(intArray + 7),(intArray + 7));
	printf("*(intArray + 8) = %d \t At Address (intArray + 8) : %p\n", *(intArray + 8),(intArray + 8));
	printf("*(intArray + 9) = %d \t At Address (intArray + 9) : %p\n", *(intArray + 9),(intArray + 9));
	ptrArray = intArray;
	
	printf("\n\n");
	printf("\t*********Using pointer as pointer i.e. value of xth element of intArray : (ptrArray + x) and address of xth element of intArray : (ptrArray + x)*********\n\n");
	printf("\tInteger array elements and their adddresses : \n\n");

	printf("ptrArray[0] = %d \t At Address &ptrArray[0] : %p\n", ptrArray[0], &ptrArray[0]);
	printf("ptrArray[1] = %d \t At Address &ptrArray[1] : %p\n", ptrArray[1], &ptrArray[1]);
	printf("ptrArray[2] = %d \t At Address &ptrArray[2] : %p\n", ptrArray[2], &ptrArray[2]);
	printf("ptrArray[3] = %d \t At Address &ptrArray[3] : %p\n", ptrArray[3], &ptrArray[3]);
	printf("ptrArray[4] = %d \t At Address &ptrArray[4] : %p\n", ptrArray[4], &ptrArray[4]);
	printf("ptrArray[5] = %d \t At Address &ptrArray[5] : %p\n", ptrArray[5], &ptrArray[5]);
	printf("ptrArray[6] = %d \t At Address &ptrArray[6] : %p\n", ptrArray[6], &ptrArray[6]);
	printf("ptrArray[7] = %d \t At Address &ptrArray[7] : %p\n", ptrArray[7], &ptrArray[7]);
	printf("ptrArray[8] = %d \t At Address &ptrArray[8] : %p\n", ptrArray[8], &ptrArray[8]);
	printf("ptrArray[9] = %d \t At Address &ptrArray[9] : %p\n", ptrArray[9], &ptrArray[9]);
	
	return (0);
}