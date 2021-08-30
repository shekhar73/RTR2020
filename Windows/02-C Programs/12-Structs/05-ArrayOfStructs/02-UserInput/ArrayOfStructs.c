#include <stdio.h>

#define NAME_LENGTH 100
#define MARITAL_STATUS 10
#define NUMBER_OF_EMPLOYEE 5

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
	//Function prototype
	void GetString(char[], int);
	
	//variable declarations
	struct Employee EmployeeData[NUMBER_OF_EMPLOYEE];
	
	int icnt;
	
	for(icnt = 0; icnt < NUMBER_OF_EMPLOYEE; icnt++)
	{
		printf("\n\n\n");
		printf("\t**********************Data entry for employee number %d*************\n", (icnt + 1));
		
		printf("\n\n");
		printf("\tEnter employee name : ");
		GetString(EmployeeData[icnt].empName, NAME_LENGTH);
		
		printf("\n\n");
		printf("\tEnter Employee's Age (in years) : ");
		scanf("%d", &EmployeeData[icnt].empAge);
		
		printf("\n\n");
		printf("\tEnter Employee's sex (M/m for Male, F/f for Female) : ");
		EmployeeData[icnt].empSex = getch();
		printf("\t%c", EmployeeData[icnt].empSex);
		EmployeeData[icnt].empSex = toupper(EmployeeData[icnt].empSex);
		
		printf("\n\n");
		printf("\tEnter employee's salary (in Indian Rupees) : ");
		scanf("%f", &EmployeeData[icnt].empSalary);
		
		printf("\n\n");
		printf("\tIs the employee married? (Y/y for Yes, N/n for No) : ");
		EmployeeData[icnt].empMaritalStatus = getch();
		printf("\t%c", EmployeeData[icnt].empMaritalStatus);
		EmployeeData[icnt].empMaritalStatus = toupper(EmployeeData[icnt].empMaritalStatus);
	}

	//Display
	printf("\n\n");
	printf("***************************Displaying employee records***********************\n\n");
	for(icnt = 0; icnt < NUMBER_OF_EMPLOYEE; icnt++)
	{
		printf("\t**********************Employee Number %d **************************\n\n", (icnt + 1));
		printf("\tName\t : %s\n", EmployeeData[icnt].empName);
		printf("\tAge\t : %d years\n", EmployeeData[icnt].empAge);
		
		if(EmployeeData[icnt].empSex == 'M')
			printf("\tSex\t : Male\n");
		else if(EmployeeData[icnt].empSex == 'F')
			printf("\tSex\t : Female\n");
		else
			printf("\tInvalid data entered\n");
		
		printf("\tSalary\t : Rs.%f\n", EmployeeData[icnt].empSalary);
		
		if(EmployeeData[icnt].empMaritalStatus == 'Y')
			printf("\Marital Status\t : Married\n");
		else if(EmployeeData[icnt].empMaritalStatus == 'N')
			printf("\tMarital Status\t\t : Unmarried\n");
		else
			printf("\tMarital Status\t : Invalid data entered\n");
		
		printf("\tMarital Status\t :%s\n", EmployeeData[icnt].empMaritalStatus);
	}
	return (0);
}

void GetString(char str[], int stringSize)
{
	//variable declarations
	int icnt;
	char ch = '\0';
	
	//code
	icnt = 0;
	do
	{
		ch = getch();
		str[icnt] = ch;
		printf("\t%c", str[icnt]);
		icnt++;
	}while((ch != '\r') && (icnt < stringSize));
	
	if(icnt == stringSize)
		str[icnt - 1] = '\0';
	else
		str[icnt] = '\0';
}