#include "tilegen.h"

#include <stdbool.h>
#include <stdlib.h>

enum Tile kBlockNone[] = { TILE_NULL };
enum Tile kBlockGrass[] = { GRASS, TILE_NULL };
enum Tile kBlockWater[] = { WATER, TILE_NULL };

enum Tile *kBlockedTransitionMap[TILE_COUNT] = {
	[TILE_NULL] = kBlockNone,
	[WATER] = kBlockGrass,
	[SAND] = kBlockNone,
	[GRASS] = kBlockWater,
};

enum Tile chunk_get(struct TileChunk* chunk, int row, int col) {
	if (0 <= row && row < TILE_CHUNK_SIZE
	 && 0 <= col && col < TILE_CHUNK_SIZE) {
		return chunk->data[row * TILE_CHUNK_SIZE + col];
	}
	return TILE_NULL;
}

void chunk_clear(struct TileChunk* chunk) {
	for (int row = 0; row < TILE_CHUNK_SIZE; row++) {
		for (int col = 0; col < TILE_CHUNK_SIZE; col++) {
			chunk->data[row * TILE_CHUNK_SIZE + col] = TILE_NULL;
		}
	}
}

static void tile_apply_blocks(bool blocked[static TILE_COUNT], enum Tile adjacent) {
	enum Tile *blocks = kBlockedTransitionMap[adjacent];
	while (*blocks != TILE_NULL) {
		blocked[*blocks] = true;
		++blocks;
	}
}

static void tile_generate(struct TileChunk* chunk, int row, int col) {
	bool blocked[TILE_COUNT] = { false };
	blocked[TILE_NULL] = true;
	tile_apply_blocks(blocked, chunk_get(chunk, row + 1, col));
	tile_apply_blocks(blocked, chunk_get(chunk, row - 1, col));
	tile_apply_blocks(blocked, chunk_get(chunk, row, col + 1));
	tile_apply_blocks(blocked, chunk_get(chunk, row, col - 1));
	
	enum Tile unblocked[TILE_COUNT] = { TILE_NULL };
	int count = 0;
	for (int i = 0; i < TILE_COUNT; i++) {
		if (!blocked[i]) {
			unblocked[count++] = (enum Tile) i;
		}
	}
	chunk->data[row * TILE_CHUNK_SIZE + col] = unblocked[rand() % count];
}

void chunk_generate(struct TileChunk* chunk, int seed) {
	chunk_clear(chunk);
	// srand(seed);
	for (int row = 0; row < TILE_CHUNK_SIZE; row++) {
		for (int col = 0; col < TILE_CHUNK_SIZE; col++) {
			tile_generate(chunk, row, col);
		}
	}
}