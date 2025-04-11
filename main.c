#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"

typedef struct {
	int a;
} Scalar;

bool compare_scalar(void* a, void* b) {
	return ((Scalar*)a)->a < ((Scalar*)b)->a;
}

static unsigned int lcg_rand(unsigned int* seed) {
	*seed = (*seed * 1103515245 + 12345) & 0x7fffffff;
	return *seed;
}

int main(void) {
	Scalar a = { 5 };
	Scalar b = { 3 };
	Scalar c = { 7 };
	Scalar d = { 1 };

	Array array;

	if (array_init(&array, 8541) != 0) {
		printf("(?) errno => %d\n", errno);
		return 1;
	}

	array_add(&array, &a);
	array_add(&array, &b);
	array_add(&array, &c);
	array_add(&array, &d);

	array_sort(&array, compare_scalar);
	
	for (size_t i = 0; i < array.size; ++i) {
		void* tmp;
		array_get(&array, i, &tmp);
		Scalar* s = (Scalar*)tmp;
		printf("%d\n", s->a);
	}

	array_destroy(&array, NULL);
	return 0;
}
