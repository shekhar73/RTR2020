#include <stdio.h>
#define MAXIMUM_STRING_LENGTH 512
int main(void)
{
	//function prototype
	int my_stings(char[]);

	char string_array[10][15] = { "Hello!", "Welcome", "To", "Real", "Time", "Rendering", "Batch", "(2020-21)", "Of", "ASTROMEDICOMP." };
	int string_length[10];
	
	int string_array_size;
	int string_array_num_rows;
	int i, j;
	
	//code
	string_array_size = sizeof(string_array);
	string_array_num_rows = string_array_size / sizeof(string_array[0]);
	
	//Storing in lengths of all the strings...
	for (i = 0; i < string_array_num_rows; i++)
	{
		string_length[i] = my_stings(string_array[i]);
	}
	
	printf("\n\n");
	printf("The entire String Array : \n\n");
	for (i = 0; i < string_array_num_rows; i++)
	{
		printf("%s ", string_array[i]);
	}
	printf("\n\n");
	printf("Strings In The 2D Array : \n\n");
	
	for (i = 0; i < string_array_num_rows; i++)
	{
		printf("String Number %d => %s\n\n", (i + 1), string_array[i]);
		for (j = 0; j < string_length[i]; j++)
		{
			printf("Character %d = %c\n", (j + 1), string_array[i][j]);
		}
		printf("\n\n");
	}
	return(0);
}
int my_stings(char str[])
{
	//variable declarations
	int j;
	int string_length = 0;

	//code
	for (j = 0; j < MAXIMUM_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);
}