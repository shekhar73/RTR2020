#include <stdio.h>
int main(void)
{
	//variable declaraions
	int int_array_one[10];
	int int_array_two[10];
	
	//code
	// ****** int_array_one[] ******
	int_array_one[0] = 4;
	int_array_one[1] = 9;
	int_array_one[2] = 3;
	int_array_one[3] = 22;
	int_array_one[4] = 1;
	int_array_one[5] = 45;
	int_array_one[6] = 76;
	int_array_one[7] = 44;
	int_array_one[8] = 77;
	int_array_one[9] = 73;
	
	printf("\n\n");
	
	printf("Piece-meal (hard-coded) assingment and display of elements to Array 'int_array_one[]': \n\n");
	printf("1st element Of Array 'int_array_one[]' Or element At 0th index Of Array 'int_array_one[]' = %d\n", int_array_one[0]);
	printf("2nd element Of Array 'int_array_one[]' Or element At 1st index Of Array 'int_array_one[]' = %d\n", int_array_one[1]);
	printf("3rd element Of Array 'int_array_one[]' Or element At 2nd index Of Array 'int_array_one[]' = %d\n", int_array_one[2]);
	printf("4th element Of Array 'int_array_one[]' Or element At 3rd index Of Array 'int_array_one[]' = %d\n", int_array_one[3]);
	printf("5th element Of Array 'int_array_one[]' Or element At 4th index Of Array 'int_array_one[]' = %d\n", int_array_one[4]);
	printf("6th element Of Array 'int_array_one[]' Or element At 5th index Of Array 'int_array_one[]' = %d\n", int_array_one[5]);
	printf("7th element Of Array 'int_array_one[]' Or element At 6th index Of Array 'int_array_one[]' = %d\n", int_array_one[6]);
	printf("8th element Of Array 'int_array_one[]' Or element At 7th index Of Array 'int_array_one[]' = %d\n", int_array_one[7]);
	printf("9th element Of Array 'int_array_one[]' Or element At 8th index Of Array 'int_array_one[]' = %d\n", int_array_one[8]);
	printf("10th element Of Array 'int_array_one[]' Or element At 9th index Of Array 'int_array_one[]' = %d\n\n", int_array_one[9]);
	
	// ****** int_array_two[] ******
	printf("\n\n");
	
	printf("Enter 1st element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[0]);
	
	printf("Enter 2nd element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[1]);
	
	printf("Enter 3rd element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[2]);
	
	printf("Enter 4th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[3]);
	
	printf("Enter 5th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[4]);
	
	printf("Enter 6th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[5]);
	
	printf("Enter 7th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[6]);
	
	printf("Enter 8th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[7]);
	
	printf("Enter 9th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[8]);
	
	printf("Enter 10th element Of Array 'int_array_two[]' : ");
	scanf("%d", &int_array_two[9]);
	
	printf("\n\n");
	printf("Piece-meal (User Input) Assignment And Display Of elements to Array 'int_array_two[]' : \n\n");
	printf("1st element Of Array 'int_array_two[]' Or element At 0th index Of Array 'int_array_two[]' = %d\n", int_array_two[0]);
	printf("2nd element Of Array 'int_array_two[]' Or element At 1st index Of Array 'int_array_two[]' = %d\n", int_array_two[1]);
	printf("3rd element Of Array 'int_array_two[]' Or element At 2nd index Of Array 'int_array_two[]' = %d\n", int_array_two[2]);
	printf("4th element Of Array 'int_array_two[]' Or element At 3rd index Of Array 'int_array_two[]' = %d\n", int_array_two[3]);
	printf("5th element Of Array 'int_array_two[]' Or element At 4th index Of Array 'int_array_two[]' = %d\n", int_array_two[4]);
	printf("6th element Of Array 'int_array_two[]' Or element At 5th index Of Array 'int_array_two[]' = %d\n", int_array_two[5]);
	printf("7th element Of Array 'int_array_two[]' Or element At 6th index Of Array 'int_array_two[]' = %d\n", int_array_two[6]);
	printf("8th element Of Array 'int_array_two[]' Or element At 7th index Of Array 'int_array_two[]' = %d\n", int_array_two[7]);
	printf("9th element Of Array 'int_array_two[]' Or element At 8th index Of Array 'int_array_two[]' = %d\n", int_array_two[8]);
	printf("10th element Of Array 'int_array_two[]' Or element At 9th index Of Array 'int_array_two[]' = %d\n\n", int_array_two[9]);
	
	return(0);
}