#include <stdio.h>
union MyUnionExample
{
	int inum;
	float fnum;
	double dnum;
	char ch;
};
int main(void)
{
	//variable declarations
	union MyUnionExample union1, union2;
	
	//code
	// *** MyUnion union2 ***
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Members Of Union union1 Are : \n\n");
	
	union1.inum = 2;
	union1.fnum = 3.2f;
	union1.dnum = 5.122333;
	union1.ch = 'S';
	
	printf("union1.inum = %d\n\n", union1.inum);
	printf("union1.fnum = %f\n\n", union1.fnum);
	printf("union1.dnum = %lf\n\n", union1.dnum);
	printf("union1.ch = %c\n\n", union1.ch);
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	printf("Addresses Of Members Of Union union1 Are : \n\n");
	printf("union1.inum = %p\n\n", &union1.inum);
	printf("union1.fnum = %p\n\n", &union1.fnum);
	printf("union1.dnum = %p\n\n", &union1.dnum);
	printf("union1.ch = %p\n\n", &union1.ch);
	printf("MyUnionExample union1 = %p\n\n", &union1);
	
	// *** MyUnionExample union2 ***
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Members Of Union union2 Are : \n\n");
	
	union2.inum = 33;
	printf("union2.inum = %dnum\n\n", union2.inum);
	
	union2.fnum = 44.5f;
	printf("union2.fnum = %fnum\n\n", union2.fnum);
	
	union2.dnum = 22.454364;
	printf("union2.dnum = %lf\n\n", union2.dnum);
	
	union2.ch = 'A';
	printf("union2.ch = %c\n\n", union2.ch);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Addresses Of Members Of Union union2 Are : \n\n");
	printf("union2.inum = %p\n\n", &union2.inum);
	printf("union2.fnum = %p\n\n", &union2.fnum);
	printf("union2.dnum = %p\n\n", &union2.dnum);
	printf("union2.ch = %p\n\n", &union2.ch);
	
	printf("MyUnionExample union2 = %p\n\n", &union2);
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	return(0);
}