#ifndef H_ARRAY
#define H_ARRAY

#include <stddef.h>
#include <errno.h>

#define ARRAY_MAX_CAPACITY (SIZE_MAX / sizeof(void*))

#define ARRAY_SUCCESS 0
#define ARRAY_ERR_INVALID_CAP -1
#define ARRAY_ERR_MEMALLOC -2

typedef struct {
	size_t size;
	size_t capacity;
	void** data;
} Array;

int array_init(Array* array, size_t initial_capacity);
void array_destroy(Array* array, void (*free_fn)(void*));

#endif // H_ARRAY
