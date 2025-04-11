#include <stdio.h>
#include <omp.h>
#include <time.h>

#include "array.h"

static inline unsigned int _lcg_rand(unsigned int* seed) {
	*seed = (*seed * 1103515245 + 12345) & 0x7fffffff;
	return *seed;
}

static inline void _quicksort_swap(void** a, void** b) {
	void* tmp = *a;
	*a = *b;
	*b = tmp;
}

static int _quicksort_partition(Array* array, size_t low, size_t high, bool (*cmp)(const void*, const void*), size_t* output) {
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
	*output = (size_t)i + 1;
	return ARRAY_SUCCESS;
}

static int _quicksort(Array* array, size_t low, size_t high, bool (*cmp)(const void*, const void*)) {
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

int array_sort(Array* array, bool (*cmp)(const void*, const void*)) {
	if (!array || !cmp) {
		return ARRAY_ERR_NULL;
	}

	if (_quicksort(array, 0, array->size - 1, cmp) != ARRAY_SUCCESS) {
		return ARRAY_ERR_INTERNAL;
	}

	return ARRAY_SUCCESS;
}
