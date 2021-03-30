#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int data;
	int choice;
	int i;

	int *a = NULL;
	int size = 0;

	do
	{
		printf("Enter an Integer:");
		scanf("%d", &data);

		a = (int*)realloc(a, (size+1)*sizeof(int));
		size = size + 1;
		a[size - 1] = data;

		printf("Entered data = %d\n", data);
		printf("Do you want to enter yet another integer: [1->Yes|No otherwise]:\n");
		scanf("%d", &choice);
	}
	while(choice == 1);

	for(i = 0; i < size; ++i)
	{
		printf("KBD:%d:%d\n", i, a[i]);
	}

	free(a);
	a = NULL;

	return (EXIT_SUCCESS);
}

