#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*dynammic_array.h*/
/*Header file -> 1*/
#define SUCCESS						1
#define ARRAY_INDEX_OUT_OF_BOUNDS 	2

#define ARR_SIZE					10

typedef int ret_t;

/*Header file -> 2*/
struct dynamic_array
{
	int *p_arr;
	int size;
};

/*Header file -> 3*/
struct dynamic_array* create_dynamic_array(int arr_size);
int get_element_at_index(struct dynamic_array* p_dynamic_array, int index, int *p_element);
int set_element_at_index(struct dynamic_array* p_dynamic_array, int index, int new_element);
int destroy_dyanamic_array(struct dynamic_array* p_dynamic_array);

/**test_dyanmic_array.c -> #include "dyanamic_array.h*/

int main(void)
{
	struct dynamic_array* p_dynamic_array = NULL;
	int i;
	int data;
	ret_t return_value;

	p_dynamic_array = create_dynamic_array(ARR_SIZE);
	for(i = 0; i < ARR_SIZE; ++i)
	{
		return_value = set_element_at_index(p_dynamic_array, i, (i + 1)*10);
		assert(return_value == SUCCESS);
	}

	for (int i = 0; i < ARR_SIZE; ++i)
	{
		/* code */
		return_value = get_element_at_index(p_dynamic_array, i, &data);
		assert(return_value == SUCCESS);
		printf("Data at index: %d: %d\n", i, data);
	}

	return_value = destroy_dyanamic_array(p_dynamic_array);
	assert(return_value == SUCCESS);
	p_dynamic_array = NULL;

	return (EXIT_SUCCESS);
}

/*dynamic_array.c -> #include "dynamic_array.h"*/
struct dynamic_array* create_dynamic_array(int arr_size)
{
	struct dynamic_array* p_dynamic_array = NULL;

	p_dynamic_array = (struct dynamic_array*)malloc(sizeof(struct dynamic_array));

	if(p_dynamic_array == NULL)
	{
		printf("Insufficient memory\n");
		exit(-1);
	}

	if(arr_size < 0)
	{
		puts("Bad size of array");
		exit(-1);
	}

	p_dynamic_array->size = arr_size;
	p_dynamic_array->p_arr = (int*)malloc(sizeof(int) * arr_size);

	if(p_dynamic_array->p_arr == NULL)
	{
		puts("Insufficient memory");
		exit(-1);
	}

	return (p_dynamic_array);
}

int get_element_at_index(struct dynamic_array* p_dynamic_array, int index, int *p_element)
{
	if(index < 0 || index >= p_dynamic_array->size)
	{
		return (ARRAY_INDEX_OUT_OF_BOUNDS);
	}
	*p_element = p_dynamic_array->p_arr[index];
	return (SUCCESS);
}

int set_element_at_index(struct dynamic_array* p_dynamic_array, int index, int new_element)
{
	if(index < 0 || index >= p_dynamic_array->size)
	{
		return(ARRAY_INDEX_OUT_OF_BOUNDS);
	}
	p_dynamic_array->p_arr[index] = new_element;
	return (SUCCESS);
}

int destroy_dyanamic_array(struct dynamic_array* p_dynamic_array)
{
	free(p_dynamic_array->p_arr);
	free(p_dynamic_array);
	return (SUCCESS);
}
