#include <stdio.h>
#define MAX_NUMBER(int_a, int_b) ((int_a > int_b) ? int_a : int_b)

int main(int argc, char *argv[], char *envp[])
{
	// variable declarations
	int int_Num01;
	int int_iNum_02;
	int int_Result;
	float float_fNum_01;
	float float_fNum_02;
	float float_fResult;
	
	// code
	// ****** COMPARING INTEGER VALUES ******
	printf("\n\n");
	printf("Enter An Integer Number : \n");
	scanf("%d", &int_Num01);
	
	printf("\n\n");
	printf("Enter Another Integer Number : \n");
	scanf("%d", &int_iNum_02);
	
	int_Result = MAX_NUMBER(int_Num01, int_iNum_02);
	
	printf("\n\n");
	printf("Result Of Macro Function MAX_NUMBER() = %d\n", int_Result);
	printf("\n\n");
	
	// ****** COMPARING FLOATING-POINT VALUES ******
	printf("\n\n");
	printf("Enter A Floating Point Number : \n");
	scanf("%f", &float_fNum_01);
	
	printf("\n\n");
	printf("Enter Another Floating Point Number : \n");
	scanf("%f", &float_fNum_02);
	
	float_fResult = MAX_NUMBER(float_fNum_01, float_fNum_02);
	
	printf("\n\n");
	printf("Result Of Macro Function MAX_NUMBER() = %f\n", float_fResult);
	
	printf("\n\n");
	
	return(0);
}