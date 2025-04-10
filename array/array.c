#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int array_init(Array* array, size_t initial_capacity) {
	if (initial_capacity > ARRAY_MAX_CAPACITY) {
		return ARRAY_ERR_INVALID_CAP;
	}

	array->size = 0;
	array->capacity = initial_capacity;
	array->data = calloc(array->capacity, sizeof(void*));

	if (!array->data) {
		return ARRAY_ERR_MEMALLOC;
	}

	return ARRAY_SUCCESS;
}

void array_destroy(Array* array, void (*free_fn)(void*)) {
	if (!array) return;
	
	if (free_fn) {
		for (size_t i = 0; i < array->size; ++i) {
			free_fn(array->data[i]);
		}
	}

	free(array->data);
	array->data = NULL;
	free(array);
}
