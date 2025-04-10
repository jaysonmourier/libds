#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int main(void) {
	Array array;

	if (array_init(&array, 8541) != 0) {
		printf("(?) errno => %d\n", errno);
		return 1;
	}
	
	array_destroy(&array, NULL);
	return 0;
}
