#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];

int mian(int argc, char *argv[])
{
	FILE *fp = NULL;

	assert(argc == 2);

	fp = fopen(argv[1], "r");
	assert(fp != NULL);

	while(fgets(buffer, BUFFER_SIZE, fp))
	{
		printf("%s\n", buffer);
		memset(buffer, 0, BUFFER_SIZE);
	}

	fclose(fp);
	fp = NULL;

	return (0);
}