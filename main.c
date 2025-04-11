#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

typedef struct {
	int a;
} Scalar;

bool compare_scalar(const void* a, const void* b) {
	return ((Scalar*)a)->a < ((Scalar*)b)->a;
}

int print_array_scalar(void* element) {
	printf("%d\n", ((Scalar*)element)->a);
	return 0;
}

int main(void) {
	Scalar a = { 5 };
	Scalar b = { 3 };
	Scalar c = { 7 };
	Scalar d = { 1 };
	Scalar e = { 9 };

	Array array;

	if (array_init(&array, 8541) != 0) {
		printf("(?) errno => %d\n", errno);
		return 1;
	}

	array_add(&array, &a);
	array_add(&array, &b);
	array_add(&array, &c);
	array_add(&array, &e);
	array_add(&array, &d);

	array_set(&array, 0, &c, NULL);

	array_sort(&array, compare_scalar);
	
	array_foreach(&array, print_array_scalar);

	array_destroy(&array, NULL);
	return 0;
}
