#include <stdio.h>

//Global typedef
typedef int INT;

int main(void)
{
	//Function prototype
	INT Add(INT, INT);
	
	//Typedefs
	typedef int INT;
	typedef float FLOAT;
	typedef char CHARACTER;
	typedef double DOUBLE;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Just like in Win32SDK~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	typedef unsigned int UINT;
	typedef UINT HANDLE;
	typedef HANDLE HWND;
	typedef HANDLE HINSTANCE;
	
	//vaiable declarations
	INT iNum = 10, icnt;
	INT iArray[] = {5, 8, 7, 6, 2, 3, 1, 2, 4, 1};
	
	FLOAT f = 3.1415f;
	const FLOAT fPI= 3.14f;
	
	CHARACTER ch = '*';
	CHARACTER chArrayOne[] = "Hello";
	CHARACTER chArrayTwo[][10] = {"RTR", "Students", "2020-2021"};
	
	DOUBLE d = 3.23343445;
	
	UINT uint = 2323;
	HANDLE handle = 343;
	HWND hwnd = 98349;
	HINSTANCE hInstance = 11122;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\tEnter INT variable inum = %d", iNum);
	printf("\n\n");
	for(icnt = 0; icnt < (sizeof(iArray) / sizeof(int)); icnt++)
	{
		printf("\tEnter my INT array variable iArray[%d] = %d\n", icnt, iArray[icnt]);
	}
	printf("\n\n");
	printf("\n\n");
	printf("\tEnter FLOAT vaiable f = %f", f);
	printf("\tEnter FLOAT constant fPI = %f", fPI);
	
	printf("\n\n");
	printf("\tEnter DOUBLE vaiable d = %lf", d);
	
	printf("\n\n");
	printf("\tEnter CHARACTER array vaiable chArrayOne = %s", chArrayOne);
	
	printf("\n\n");
	for(icnt = 0; icnt < (sizeof(chArrayTwo) / sizeof(chArrayTwo[0])); icnt++)
	{
		printf("%s\t", chArrayTwo[icnt]);
	}
	printf("\n\n");
	
	printf("\tEnter UINT vaiable uint = %u\n\n", uint);
	printf("\tEnter HANDLE vaiable handle = %u\n\n", handle);
	printf("\tEnter HWND vaiable hwnd = %u\n\n", hwnd);
	printf("\tEnter HINSTANCE variable = %u\n\n", hInstance);
	printf("\n\n");
	
	INT x = 43;
	INT y = 89;
	INT result;
	
	result = Add(x, y);
	printf("\tResult = %d\n\n", result);
	
	
	return (0);
}

INT Add(INT iNum1, INT iNum2)
{
	INT Ans;
	Ans = iNum1 + iNum2;
	return(Ans);
}