#include <stdio.h>

struct Employee
{
	char myName[100];
	int myAge;
	float mySalray;
	char mySex;
	char myMaritalStatus;
};

int main(void)
{
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("\tSizes of data types and pointers to those respective data types are : \n\n");
	
	printf("\n\n*********************************************Size of int and Integer pointers************************************************\n\n");
	printf("\tSize of (int) : %d\t\t Size of pointer to int (int*) : %d\t\tSize of pointer to pointer to int (int**) : %d\t\t", 
		sizeof(int), sizeof(int*), sizeof(int**));
	
	
	printf("\n\n*********************************************Size of float and float pointers************************************************\n\n");
	
	printf("\tSize of (float) : %d\t\t Size of pointer to float (float*) : %d \t\tSize of pointer to pointer to float (float**) : %d\t\t", 
		sizeof(float), sizeof(float*), sizeof(float**));
		
	
	printf("\n\n*********************************************Size of double and double pointers************************************************\n\n");
	
	printf("\tSize of (double) : %d\t\t Size of pointer to double (double*) : %d\t\tSize of pointer to pointer to double (double**) : %d\t\t", 
		sizeof(double), sizeof(double*), sizeof(double**));
	
	
	printf("\n\n*********************************************Size of char and character pointers************************************************\n\n");
	
	printf("\tSize of (char) : %d\t\t Size of pointer to char (char*) : %d\t\tSize of pointer to pointer to char (char**) : %d\t\t", 
		sizeof(char), sizeof(char*), sizeof(char**));
		
	
	printf("\n\n*********************************************Size of struct and structure pointers************************************************\n\n");
	
	printf("\tSize of (struct Employee) : %d\t\t Size of pointer to struct Employee (struct Employee*) : %d\t\tSize of pointer to pointer to struct Employee (struct Employee**) : %d\t\t", 
		sizeof(struct Employee), sizeof(struct Employee*), sizeof(struct Employee**));
	
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	
	return (0);
}