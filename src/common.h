#include <stdio.h>
#include <stdlib.h>

#define ASSERT(x) do { \
    if (!(x)) { \
		printf("Failed assertion %s on line %d\n", #x, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0);

#define JMAX(x, y) ( ((x) > (y)) ? (x) : (y) )

#define JMIN(x, y) ( ((x) < (y)) ? (x) : (y) )