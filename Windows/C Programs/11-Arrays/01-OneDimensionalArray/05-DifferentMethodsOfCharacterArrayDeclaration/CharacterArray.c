#include <stdio.h>
int main(void)
{
	//variable declarations
	char char_array_01[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P', '\0' }; // Must give \0 Explicitly For Proper Initialization
	char char_array_02[9] = { 'W', 'E', 'L', 'C', 'O', 'M', 'E', 'S', '\0' }; // Must give \0 Explicitly For Proper Initialization
	char char_array_03[] = { 'Y', 'O', 'U', '\0' }; // Must give \0 Explicitly For Proper Initialization
	char char_array_04[] = "To"; // \0 is assumed, size is given as 3, although string has only 2 characters
	char char_array_05[] = "REAL TIME RENDERING BATCH OF 2020-21"; // \0 is assumed, size is given as 40, although string has 39 characters
	char char_array_WithoutNullTerminator[] = { 'H', 'e', 'l', 'l', 'o' };
	
	//code
	printf("\n\n");
	printf("Size Of char_array_01 : %lu\n\n", sizeof(char_array_01));
	printf("Size Of char_array_02 : %lu\n\n", sizeof(char_array_02));
	printf("Size Of char_array_03 : %lu\n\n", sizeof(char_array_03));
	printf("Size Of char_array_04 : %lu\n\n", sizeof(char_array_04));
	printf("Size Of char_array_05 : %lu\n\n", sizeof(char_array_05));
	
	printf("\n\n");
	printf("The string are : \n\n");
	printf("char_array_01 : %s\n\n", char_array_01);
	printf("char_array_02 : %s\n\n", char_array_02);
	printf("char_array_03 : %s\n\n", char_array_03);
	printf("char_array_04 : %s\n\n", char_array_04);
	printf("char_array_05 : %s\n\n", char_array_05);
	printf("\n\n");
	printf("Size of char_array_WithoutNullTerminator : %lu\n\n", sizeof(char_array_WithoutNullTerminator));
	printf("char_array_WithoutNullTerminator : %s\n\n",char_array_WithoutNullTerminator); //Will display garbage value at the end of string due to absence of \0
	return(0);
}