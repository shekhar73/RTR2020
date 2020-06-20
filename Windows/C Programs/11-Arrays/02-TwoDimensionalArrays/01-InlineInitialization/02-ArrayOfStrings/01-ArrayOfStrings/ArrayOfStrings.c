#include <stdio.h>
#define MAX_STRING_LENGTH 512
int main(void)
{
	//function prototype
	int str_length(char[]);
	//variable declaraions
	

	char string_array[10][15] = { "Hello!", "Welcome", "To", "Real", "Time",
	"Rendering", "Batch", "(2020-21)", "Of", "ASTROMEDICOMP." }; 
	int char_size;
	int string_array_size;
	int string_array_num_elements, string_array_num_rows, string_array_num_columns;
	int strActual_num_chars = 0;
	int i;
	
	//code
	printf("\n\n");
	char_size = sizeof(char);
	string_array_size = sizeof(string_array);
	printf("Size Of Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_size);
	
	string_array_num_rows = string_array_size / sizeof(string_array[0]);
	printf("Number of Rows (Strings) In Two Dimensional ( 2D ) Character Array(String Array) Is = %d\n\n", string_array_num_rows);
	
	string_array_num_columns = sizeof(string_array[0]) / char_size;
	printf("Number of Columns In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_columns);
	
	string_array_num_elements = string_array_num_rows * string_array_num_columns;
	printf("Maximum Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", string_array_num_elements);
	for (i = 0; i < string_array_num_rows; i++)
	{
		strActual_num_chars = strActual_num_chars + str_length(string_array[i]);
	}
	
	printf("Actual Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strActual_num_chars);
	printf("\n\n");
	printf("Strings In The 2D Array : \n\n");
	
	printf("%s ", string_array[0]);
	printf("%s ", string_array[1]);
	printf("%s ", string_array[2]);
	printf("%s ", string_array[3]);
	printf("%s ", string_array[4]);
	printf("%s ", string_array[5]);
	printf("%s ", string_array[6]);
	printf("%s ", string_array[7]);
	printf("%s ", string_array[8]);
	printf("%s\n\n", string_array[9]);
	return(0);
}

int str_length(char str[])
{
	//variable declarations
	int j;
	int string_length = 0;
	//code

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	
	return(string_length);
}