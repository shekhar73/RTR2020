#include <stdio.h>

#define NAME_LENGTH 100
#define MARITAL_STATUS 10


struct Employee
{
	char	empName[NAME_LENGTH];
	int 	empAge;
	float 	empSalary;
	char 	empSex;
	char 	empMaritalStatus[MARITAL_STATUS];
};

int main(void)
{
	//variable declarations
	struct Employee EmployeeData[5];
	
	char employeeSagar[] = "Sagar";
	char employeeMahesh[] = "Mahesh";
	char employeeAkshay[] = "Akshay";
	char employeeBaba[] = "Baba";
	char employeeKiran[] = "Kiran";
	
	int icnt;
	
	//code 
	
	//Hardcode initialization of array of 'struct Employee'
	
	//Employee 1
	strcpy(EmployeeData[0].empName, employeeSagar);
	EmployeeData[0].empAge = 27;
	EmployeeData[0].empSex = 'M';
	EmployeeData[0].empSalary = 35000.0f;
	strcpy(EmployeeData[0].empMaritalStatus, "Unmarried");
	
	//Employee 2
	strcpy(EmployeeData[1].empName, employeeMahesh);
	EmployeeData[1].empAge = 26;
	EmployeeData[1].empSex = 'M';
	EmployeeData[1].empSalary = 34000.0f;
	strcpy(EmployeeData[1].empMaritalStatus, "Unmarried");
	
	//Employee 3
	strcpy(EmployeeData[2].empName, employeeAkshay);
	EmployeeData[2].empAge = 27;
	EmployeeData[2].empSex = 'M';
	EmployeeData[2].empSalary = 44400.0f;
	strcpy(EmployeeData[2].empMaritalStatus, "Married");
	
	//Employee 4
	strcpy(EmployeeData[3].empName, employeeBaba);
	EmployeeData[3].empAge = 27;
	EmployeeData[3].empSex = 'M';
	EmployeeData[3].empSalary =55400.0f;
	strcpy(EmployeeData[3].empMaritalStatus, "Married");
	
	//Employee 5
	strcpy(EmployeeData[4].empName, employeeKiran);
	EmployeeData[4].empAge = 26;
	EmployeeData[4].empSex = 'M';
	EmployeeData[4].empSalary = 34400.0f;
	strcpy(EmployeeData[4].empMaritalStatus, "Married");
	
	//Display
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Displaying employee data~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	for(icnt = 0; icnt < 5; icnt++)
	{
		printf("\t**********************Employee Number %d **************************\n\n", (icnt + 1));
		printf("\tName\t : %s\n", EmployeeData[icnt].empName);
		printf("\tAge\t : %d years\n", EmployeeData[icnt].empAge);
		
		if(EmployeeData[icnt].empSex == 'M' || EmployeeData[icnt].empSex == 'm')
			printf("\tSex\t : Male\n");
		else
			printf("\tSex\t : Female\n");
		
		printf("\tSalary\t : Rs.%f\n", EmployeeData[icnt].empSalary);
		printf("\tMarital Status\t :%s\n", EmployeeData[icnt].empMaritalStatus);
			
	}
	
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	return(0);
	
}