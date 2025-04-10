#ifndef H_ARRAY
#define H_ARRAY

#include <stddef.h>
#include <errno.h>

#define ARRAY_MAX_CAPACITY (SIZE_MAX / sizeof(void*))
#define ARRAY_GROWTH_FACTOR 2

#define ARRAY_SUCCESS 0
#define ARRAY_ERR_INVALID_CAP -1
#define ARRAY_ERR_MEMALLOC -2
#define ARRAY_ERR_OUT_OF_BOUNDS -3
#define ARRAY_ERR_NULL -4
#define ARRAY_ERR_INTERNAL -5

typedef struct {
	size_t size;
	size_t capacity;
	void** data;
} Array;

int array_init(Array* array, size_t initial_capacity);
void array_destroy(Array* array, void (*free_fn)(void*));

int array_add(Array* array, void* element);
int array_get(Array* array, unsigned index, void** element);
int array_set(Array* array, unsigned index, void* new_element, void (*free_fn)(void*));

#endif // H_ARRAY
