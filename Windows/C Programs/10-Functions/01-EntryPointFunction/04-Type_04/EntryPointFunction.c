#include <stdio.h>
// Entry-Point Function => main() => Valid Return Type (int) and 2 Parameters (int argc, char *argv[])
int main(int argc, char *argv[])
{
	//variable declarations
	int inum;
	
	//code
	printf("\n\n");
	printf("Hello World !!!\n\n"); //Library function
	printf("Number Of Command Line Arguments = %d\n\n", argc);
	printf("Command Line Arguments Passed To This Program Are : \n\n");
	
	for (inum = 0; inum < argc; inum++)
	{
		printf("Command Line Argument Number %d = %s\n", (inum + 1), argv[inum]);
	}
	printf("\n\n");
	return(0);
}