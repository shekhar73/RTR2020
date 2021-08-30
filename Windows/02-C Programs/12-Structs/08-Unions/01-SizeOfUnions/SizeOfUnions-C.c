#include <stdio.h>

struct MyStruct
{
	int inum;
	float fnum;
	double dnum;
	char ch;
};
union MyUnion
{
	int inum;
	float fnum;
	double dnum;
	char ch;
};

int main(void)
{
	//variable declarations
	struct MyStruct obj1;
	union MyUnion obj2;
	
	//code
	printf("\n\n");
	printf("Size Of MyStruct = %lu\n", sizeof(obj1));
	printf("\n\n");
	printf("Size Of MyUnion = %lu\n", sizeof(obj2));
	return(0);
}