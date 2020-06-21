#include <stdio.h>

#define MAXIMUM_NAME_LENGTH 100

struct Employee
{
	char name[MAXIMUM_NAME_LENGTH];
	unsigned int sk_age;
	char sk_gender;
	double sk_salary;
};

struct MyData
{
	int iNum;
	float fNum;
	double dNum;
	char ch;
};

int main(void)
{
	//Typedefs
	typedef struct Employee MY_EMPLOYEE_TYPE;
	typedef struct MyData MY_DATA_TYPE;
	
	//variable declarations
	struct Employee structEmployee = {"SSK", 26, 'M', 11122.00};
	MY_EMPLOYEE_TYPE empyTypedef = {"Tesla", 25, 'M', 23343.00};

	struct MyData structMd = {34, 22.33f, 23.22221, 'x'};
	MY_DATA_TYPE structMDTypedef;
	
	//code
	structMDTypedef.iNum = 3;
	structMDTypedef.fNum = 3.232f;
	structMDTypedef.dNum = 4.3434343;
	structMDTypedef.ch = 'S';
	
	printf("\n\n");
	printf("\tStruct Employee");
	printf("structEmployee.name = %s\n", structEmployee.name);
	printf("structEmployee.sk_age = %d\n", structEmployee.sk_age);
	printf("structEmployee.sk_gender = %c\n", structEmployee.sk_gender);
	printf("structEmployee.sk_salary = %lf\n", structEmployee.sk_salary);
	printf("\n\n");
	printf("MY_EMPLOYEE_TYPE : \n\n");
	printf("empyTypedef.name = %s\n", empyTypedef.name);
	printf("empyTypedef.sk_age = %d\n", empyTypedef.sk_age);
	printf("empyTypedef.sk_gender = %c\n", empyTypedef.sk_gender);
	printf("empyTypedef.sk_salary = %lf\n", empyTypedef.sk_salary);
	
	printf("\n\n");
	printf("struct MyData : \n\n");
	printf("structMd.iNum = %d\n", structMd.iNum);
	printf("structMd.fNum = %f\n", structMd.fNum);
	printf("structMd.dNum = %lf\n", structMd.dNum);
	printf("structMd.ch = %c\n", structMd.ch);
	printf("\n\n");
	printf("MY_DATA_TYPE : \n\n");
	printf("structMDTypedef.iNum = %d\n", structMDTypedef.iNum);
	printf("structMDTypedef.fNum = %f\n", structMDTypedef.fNum);
	printf("structMDTypedef.dNum = %lf\n", structMDTypedef.dNum);
	printf("structMDTypedef.ch = %c\n", structMDTypedef.ch);
	printf("\n\n");
	return(0);
}