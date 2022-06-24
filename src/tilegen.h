#include <stdint.h>

#define TILE_CHUNK_SIZE 100

enum Tile {
	TILE_NULL = 0,
	WATER,
	SAND,
	GRASS,
	TILE_COUNT,
};

struct TileChunk {
	enum Tile data[TILE_CHUNK_SIZE * TILE_CHUNK_SIZE];
};

// Returns TILE_NULL if row or col is out of bounds.
enum Tile chunk_get(struct TileChunk* chunk, int row, int col);

void chunk_clear(struct TileChunk* chunk);

void chunk_generate(struct TileChunk* chunk, int seed);

