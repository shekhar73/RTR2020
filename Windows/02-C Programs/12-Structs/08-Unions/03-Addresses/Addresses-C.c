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
	struct MyStruct structObj;
	union MyUnion uObj;
	
	//code
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Members Of Struct Are : \n\n");
	
	structObj.inum = 12;
	structObj.fnum = 33.2f;
	structObj.dnum = 53.1444433;
	structObj.ch = 'S';
	
	printf("structObj.inum = %d\n\n", structObj.inum);
	printf("structObj.fnum = %f\n\n", structObj.fnum);
	printf("structObj.dnum = %lf\n\n", structObj.dnum);
	printf("structObj.ch = %c\n\n", structObj.ch);
	
	printf("Addresses Of Members Of Struct Are : \n\n");
	printf("structObj.inum = %p\n\n", &structObj.inum);
	printf("structObj.fnum = %p\n\n", &structObj.fnum);
	printf("structObj.dnum = %p\n\n", &structObj.dnum);
	printf("structObj.ch = %p\n\n", &structObj.ch);
	
	printf("MyStruct structObj = %p\n\n", &structObj);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Members Of Union Are : \n\n");
	uObj.inum = 32;
	printf("uObj.inum = %d\n\n", uObj.inum);
	
	uObj.fnum = 22.5f;
	printf("uObj.fnum = %f\n\n", uObj.fnum);
	
	uObj.dnum = 53.5345345;
	printf("uObj.dnum = %lf\n\n", uObj.dnum);
	
	uObj.ch = 'A';
	printf("uObj.ch = %c\n\n", uObj.ch);
	
	printf("Addresses Of Members Of Union Are : \n\n");
	printf("uObj.inum = %p\n\n", &uObj.inum);
	printf("uObj.fnum = %p\n\n", &uObj.fnum);
	printf("uObj.dnum = %p\n\n", &uObj.dnum);
	printf("uObj.ch = %p\n\n", &uObj.ch);
	printf("MyUnion uObj = %p\n\n", &uObj);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return(0);
}