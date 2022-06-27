#include <stdio.h>
#include <stdlib.h>

#define ASSERT(x) do { \
    if (!(x)) { \
		printf("Failed assertion %s on line %d\n", #x, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0);

