#include <stdio.h>
struct MyData
{
	int inum;
	float fnum;
	double dnum;
	char ch;
};

int main(void)
{
	//variable declarations
	struct MyData data;
	
	//code
	//Assigning Data Values To The Data Members Of 'struct MyData'
	data.inum = 45;
	data.fnum = 22.45f;
	data.dnum = 14.2444995;
	data.ch = 'S';
	
	//Displaying Values Of The Data Members Of 'struct MyData'
	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("inum = %dnum\n", data.inum);
	printf("fnum = %fnum\n", data.fnum);
	printf("dnum = %lf\n", data.dnum);
	printf("ch = %ch\n\n", data.ch);
	printf("\n\n");
	
	printf("ADDRESSES OF DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("'inum' occupies addresses from %p\n", &data.inum);
	printf("'fnum' occupies addresses from %p\n", &data.fnum);
	printf("'dnum' occupies addresses from %p\n", &data.dnum);
	printf("'ch' occupies Address %p\n\n", &data.ch);
	printf("Starting Address Of 'struct MyData' variable 'data' = %p\n\n", &data);
	
	return(0);
}