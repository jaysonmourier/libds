#ifndef H_LIBDS_ARRAY
#define H_LIBDS_ARRAY

#include <stddef.h>
#include <errno.h>

#define ARRAY_MAX_CAPACITY (SIZE_MAX / sizeof(void*))
#define ARRAY_GROWTH_FACTOR 2

#define ARRAY_FOREACH_BREAK 1
#define ARRAY_SUCCESS 0
#define ARRAY_ERR_INVALID_CAP -1
#define ARRAY_ERR_MEMALLOC -2
#define ARRAY_ERR_OUT_OF_BOUNDS -3
#define ARRAY_ERR_NULL -4
#define ARRAY_ERR_INTERNAL -5

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		size_t size;
		size_t capacity;
		void** data;
	} Array;

	/*
	* Initializes a new empty array
	*
	* @param array Pointer to the array structure
	* @param initial_capacity Initial capacity of the array
	* @return ARRAY_SUCCESS, ARRAY_ERR_INVALID_CAP or ARRAY_ERR_MEMALLOC
	*/
	int array_init(Array* array, size_t initial_capacity);

	/*
	* Destroys an array
	*
	* @param array Pointer to the array structure
	* @param free_fn Pointer to the function used to free the data
	*/
	void array_destroy(Array* array, void (*free_fn)(void*));

	/*
	* Pushes the element at the end of the array
	*
	* @param array Pointer to the array structure
	* @param element Pointer to the element to push at the end of the array
	* @return ARRAY_SUCCESS, ARRAY_ERR_NULL or ARRAY_ERR_INTERNAL
	*/
	int array_add(Array* array, void* element);

	/*
	* Gets the element from the array at the given index
	*
	* @param array Pointer to the array structure
	* @param index Index of the element
	* @param element Output pointer where the retrieved element will be stored
	* @return ARRAY_SUCCESS, ARRAY_ERR_NULL or ARRAY_ERR_OUT_OF_BOUNDS
	*/
	int array_get(Array* array, unsigned index, void** element);

	/*
	* Removes the element from the array at the given index
	* The function maintains element order by shifting subsequent elements to the left.
	* The last slot is cleared (set to NULL) and the size is decremented.
	* If element order is not important, consider using `array_remove_fast()` for better performance.
	* 
	* @param array Pointer to the array structure
	* @param index Index of the element
	* @param free_fn Pointer to the function used to free the element at the given index
	* @return ARRAY_SUCCESS, ARRAY_ERR_NULL or ARRAY_ERR_OUT_OF_BOUNDS
	*/
	int array_remove(Array* array, unsigned index, void (*free_fn)(void*));

	/*
	* Removes the element from the array at the given index
	*
	* @param array Pointer to the array structure
	* @param index Index of the element
	* @param free_fn Pointer to the function used to free the element at the given index
	* @return ARRAY_SUCCESS, ARRAY_ERR_NULL or ARRAY_ERR_OUT_OF_BOUNDS
	*/
	int array_remove_fast(Array* array, unsigned index, void (*free_fn)(void*));

	/*
	* Sets the element from array at the given index
	*
	* @param array Pointer to the array structure
	* @param index Index at which to set the new element
	* @param new_element Pointer to the new element
	* @param free_fn Function used to free the old element at the given index
	* @return ARRAY_SUCCESS, ARRAY_ERR_NULL or ARRAY_ERR_OUT_OF_BOUNDS
	*/
	int array_set(Array* array, unsigned index, void* new_element, void (*free_fn)(void*));

	/*
	* Applies a function to each element of the array
	* 
	* @param array Pointer to the array structure
	* @param callback Pointer to the function to apply to each element
	* @return ARRAY_SUCCESS, ARRAY_ERR_NULL or the value returned by the callback function
	*/
	int array_foreach(Array* array, int (*callback)(void*));

#ifdef __cplusplus
}
#endif

#endif // H_LIBDS_ARRAY
