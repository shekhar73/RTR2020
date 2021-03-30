#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *line = "100, 200, 300";

int main()
{
	char *token = NULL;
	char *sep = ",";

	token = strtok(line, sep);
	printf("token = %s\n", token);

	token = strtok(NULL, sep);
	printf("token = %s\n", token);

	token = strtok(NULL, sep);
	printf("token = %s\n", token);

	return (0);
}
