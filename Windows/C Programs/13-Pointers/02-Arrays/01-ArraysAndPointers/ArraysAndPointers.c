#include <stdio.h>

int main(void)
{
	//variable declrations
	int intArray[] = {12, 24, 36, 48, 60, 72, 84, 96, 108, 120};
	float floatArray[] = {3.4f, 5.3f, 2.3f, 6.4f, 22.2f};
	double doubleArray[] = {1.22222222, 4.44444444444,labs 6.6666666666};
	char chArray[] = {'S', 'h', 'e', 'k', 'h', 'a', 'r', '\0'};
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t Integer array elements and the addresses they occuepy are as follows : \n\n");
	
	printf("\t intArray[0] = %d \t at address : %p\n", *(intArray + 0), (intArray + 0));
	printf("\t intArray[1] = %d \t at address : %p\n", *(intArray + 1), (intArray + 1));
	printf("\t intArray[2] = %d \t at address : %p\n", *(intArray + 2), (intArray + 2));
	printf("\t intArray[3] = %d \t at address : %p\n", *(intArray + 3), (intArray + 3));
	printf("\t intArray[4] = %d \t at address : %p\n", *(intArray + 4), (intArray + 4));
	printf("\t intArray[5] = %d \t at address : %p\n", *(intArray + 5), (intArray + 5));
	printf("\t intArray[6] = %d \t at address : %p\n", *(intArray + 6), (intArray + 6));
	printf("\t intArray[7] = %d \t at address : %p\n", *(intArray + 7), (intArray + 7));
	printf("\t intArray[8] = %d \t at address : %p\n", *(intArray + 8), (intArray + 8));
	printf("\t intArray[9] = %d \t at address : %p\n", *(intArray + 9), (intArray + 9));
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\t Float array elements and the addresses they occuepy are as follows : \n\n");
	printf("\t floatArray[0] = %f \t at address : %p\n", *(floatArray + 0), (floatArray + 0));
	printf("\t floatArray[1] = %f \t at address : %p\n", *(floatArray + 1), (floatArray + 1));
	printf("\t floatArray[2] = %f \t at address : %p\n", *(floatArray + 2), (floatArray + 2));
	printf("\t floatArray[3] = %f \t at address : %p\n", *(floatArray + 3), (floatArray + 3));
	printf("\t floatArray[4] = %f \t at address : %p\n", *(floatArray + 4), (floatArray + 4));

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	printf("\t Double array elements and the addresses they occuepy are as follows : \n\n");
	printf("\t doubleArray[0] = %lf \t at address : %p\n", *(doubleArray + 0), (doubleArray + 0));
	printf("\t doubleArray[1] = %lf \t at address : %p\n", *(doubleArray + 1), (doubleArray + 1));
	printf("\t doubleArray[2] = %lf \t at address : %p\n", *(doubleArray + 2), (doubleArray + 2));
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	printf("\t Character array elements and the addresses they occuepy are as follows : \n\n");
	printf("\t chArray[0] = %c \t at address : %p\n", *(chArray + 0), (chArray + 0));
	printf("\t chArray[1] = %c \t at address : %p\n", *(chArray + 1), (chArray + 1));
	printf("\t chArray[2] = %c \t at address : %p\n", *(chArray + 2), (chArray + 2));
	printf("\t chArray[3] = %c \t at address : %p\n", *(chArray + 3), (chArray + 3));
	printf("\t chArray[4] = %c \t at address : %p\n", *(chArray + 4), (chArray + 4));
	printf("\t chArray[5] = %c \t at address : %p\n", *(chArray + 5), (chArray + 5));
	printf("\t chArray[6] = %c \t at address : %p\n", *(chArray + 6), (chArray + 6));
	
	return (0);
}