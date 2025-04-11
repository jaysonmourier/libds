#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
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

int array_size(const Array* array, size_t* size_output) {
	if (!array || !size_output) {
		return ARRAY_ERR_NULL;
	}
	*size_output = array->size;
	return ARRAY_SUCCESS;
}

int array_capacity(const Array* array, size_t* capacity_output) {
	if (!array || !capacity_output) {
		return ARRAY_ERR_NULL;
	}
	*capacity_output = array->capacity;
	return ARRAY_SUCCESS;
}

int array_get(const Array* array, const unsigned index, void** element) {
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

int array_set(Array* array, const unsigned index, void* new_element, void (*free_fn)(void*)) {
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

int array_remove(Array* array, const unsigned index, void (*free_fn)(void*)) {
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

int array_remove_fast(Array* array, const unsigned index, void (*free_fn)(void*)) {
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

int array_clear(Array* array, void (*free_fn)(void*)) {
	if (!array || !array->data) {
		return ARRAY_ERR_NULL;
	}

	for (size_t i = 0; i < array->size; ++i) {
		if (free_fn && array->data[i]) {
			free_fn(array->data[i]);
		}
		array->data[i] = NULL;
	}

	array->size = 0;

	return ARRAY_SUCCESS;
}

int array_find_first(const Array* array, const void* element, bool (*cmp)(void*, void*), size_t* index_output) {
	if (!array || !element || !cmp || !index_output) {
		return ARRAY_ERR_NULL;
	}

	for (size_t i = 0; i < array->size; ++i) {
		if (cmp(element, array->data[i])) {
			*index_output = i;
			return ARRAY_SUCCESS;
		}
	}

	return ARRAY_ERR_NOT_FOUND;
}

int array_find_last(const Array* array, const void* element, bool (*cmp)(void*, void*), size_t* index_output) {
	if (!array || !element || !cmp || !index_output) {
		return ARRAY_ERR_NULL;
	}

	for (size_t i = array->size; i-- > 0;) {
		if (cmp(element, array->data[i])) {
			*index_output = i;
			return ARRAY_SUCCESS;
		}
	}

	return ARRAY_ERR_NOT_FOUND;
}

static inline void _quicksort_swap(void** a, void** b) {
	void* tmp = *a;
	*a = *b;
	*b = tmp;
}

static inline unsigned int _lcg_rand(unsigned int* seed) {
	*seed = (*seed * 1103515245 + 12345) & 0x7fffffff;
	return *seed;
}

static int _quicksort_partition(Array *array, size_t low, size_t high, bool (*cmp)(void*,void*), size_t* output) {
	if (!array || !cmp) {
		return ARRAY_ERR_NULL;
	}

	if ((low > high) || (high >= array->size)) {
		return ARRAY_ERR_OUT_OF_BOUNDS;
	}

	void* pivot = array->data[high];
	int32_t i = (int32_t)low - 1;

	for (size_t j = low; j < high; ++j) {
		if (cmp(array->data[j], pivot)) {
			i++;
			_quicksort_swap(&array->data[i], &array->data[j]);
		}
	}

	_quicksort_swap(&array->data[i + 1], &array->data[high]);
	*output = (size_t) i + 1;
	return ARRAY_SUCCESS;
}

static int _quicksort(Array* array, size_t low, size_t high, bool (*cmp)(void*, void*)) {
	if (!array || !cmp) {
		return ARRAY_ERR_NULL;
	}

	if (low >= high || high >= array->size) {
		return ARRAY_SUCCESS;
	}

	int thread_id = omp_in_parallel() ? omp_get_thread_num() : 0;
	unsigned int seed = (unsigned int)(clock() + 5678 * thread_id + low + high); // thread safe
	size_t pivot_index = low + _lcg_rand(&seed) % (high - low + 1);
		
	_quicksort_swap(&array->data[pivot_index], &array->data[high]);

	if (_quicksort_partition(array, low, high, cmp, &pivot_index) != ARRAY_SUCCESS) {
		return ARRAY_ERR_INTERNAL;
	}
	
	if ((high - low) > ARRAY_QS_PARALLEL_THRESHOLD) {
		int status = ARRAY_SUCCESS;
		
		#pragma omp parallel sections shared(status)
		{
			#pragma omp section
			if (_quicksort(array, low, pivot_index - 1, cmp) != ARRAY_SUCCESS) {
				#pragma omp critical
				status = ARRAY_ERR_INTERNAL;
			}

			#pragma omp section
			if (_quicksort(array, pivot_index + 1, high, cmp) != ARRAY_SUCCESS) {
				#pragma omp critical
				status = ARRAY_ERR_INTERNAL;
			}
		}

		if (status != ARRAY_SUCCESS) return status;
	}
	else {
		if (_quicksort(array, low, pivot_index - 1, cmp) != ARRAY_SUCCESS) {
			return ARRAY_ERR_INTERNAL;
		}

		if (_quicksort(array, pivot_index + 1, high, cmp) != ARRAY_SUCCESS) {
			return ARRAY_ERR_INTERNAL;
		}
	}

	return ARRAY_SUCCESS;
}

int array_sort(Array* array, bool (*cmp)(void*, void*)) {
	if (!array || !cmp) {
		return ARRAY_ERR_NULL;
	}

	srand(time(NULL));

	if (_quicksort(array, 0, array->size - 1, cmp) != ARRAY_SUCCESS) {
		return ARRAY_ERR_INTERNAL;
	}

	return ARRAY_SUCCESS;
}
