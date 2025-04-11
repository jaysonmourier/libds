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
	array->size = 0;
	array->capacity = 0;
}

int array_get(Array* array, unsigned index, void** element) {
	if (!array || !array->data || !element) {
		return ARRAY_ERR_NULL;
	}
	
	if (index >= array->size) {
		*element = NULL;
		return ARRAY_ERR_OUT_OF_BOUNDS;
	}

	*element = array->data[index];
	return ARRAY_SUCCESS;
}

int array_set(Array* array, unsigned index, void* new_element, void (*free_fn)(void*)) {
	if (!array || !array->data) {
		return ARRAY_ERR_NULL;
	}

	if (index >= array->size) {
		return ARRAY_ERR_OUT_OF_BOUNDS;
	}

	if (free_fn && array->data[index]) {
		free_fn(array->data[index]);
	}

	array->data[index] = new_element;
	return ARRAY_SUCCESS;
}

/*
* Resizes the memory of the array to match the new capacity
* 
* @param array Pointer to the array structure
* @param new_capacity New capacity of the array
* @return ARRAY_SUCCESS, ARRAY_ERR_NULL, ARRAY_ERR_INVALID_CAP or ARRAY_ERR_MEMALLOC
*/
static int _array_resize(Array* array, size_t new_capacity) {
	if (!array || !array->data) {
		return ARRAY_ERR_NULL;
	}

	if (new_capacity < array->size) {
		return ARRAY_ERR_INVALID_CAP;
	}

	void** new_mem_block = realloc(array->data, new_capacity * sizeof(void*));

	if (!new_mem_block) {
		return ARRAY_ERR_MEMALLOC;
	}
	
	array->capacity = new_capacity;
	array->data = new_mem_block;
	return ARRAY_SUCCESS;
}

int array_add(Array* array, void* element) {
	if (!array || !array->data) {
		return ARRAY_ERR_NULL;
	}

	if (array->size == array->capacity) {
		size_t new_capacity = array->capacity == 0 ? 1 : array->capacity * ARRAY_GROWTH_FACTOR;

		if (_array_resize(array, new_capacity) != ARRAY_SUCCESS) {
			return ARRAY_ERR_INTERNAL;
		}
	}

	array->data[array->size++] = element;
	return ARRAY_SUCCESS;
}

int array_remove(Array* array, unsigned index, void (*free_fn)(void*)) {
	if (!array) {
		return ARRAY_ERR_NULL;
	}

	if (index >= array->size) {
		return ARRAY_ERR_OUT_OF_BOUNDS;
	}

	if (free_fn) {
		free_fn(array->data[index]);
		array->data[index] = NULL;
	}

	size_t current_index = index;
	for (size_t i = current_index + 1; i < array->size; ++i) {
		array->data[current_index++] = array->data[i];
	}

	array->size--;
	array->data[array->size] = NULL;

	return ARRAY_SUCCESS;
}

int array_remove_fast(Array* array, unsigned index, void (*free_fn)(void*)) {
	if (!array) {
		return ARRAY_ERR_NULL;
	}

	if (index >= array->size) {
		return ARRAY_ERR_OUT_OF_BOUNDS;
	}

	array->size--;

	if (free_fn && array->data[index]) {
		free_fn(array->data[index]);
	}

	if (index != array->size) {
		array->data[index] = array->data[array->size];
	}

	array->data[index] = NULL;

	return ARRAY_SUCCESS;
}

int array_foreach(Array* array, int (*fn)(void*)) {
	if (!array || !array->data || !fn) {
		return ARRAY_ERR_NULL;
	}

	for (size_t i = 0; i < array->size; ++i) {
		int result = fn(array->data[i]);
		if (result == ARRAY_FOREACH_BREAK) break;
		else if (result != ARRAY_SUCCESS) return result;
	}

	return ARRAY_SUCCESS;
}
