#include "tilegen.h"

#include <stdbool.h>
#include <stdlib.h>

// #include <stdio.h>

int kTileWeightMatrix[TILE_COUNT][TILE_COUNT] = {
	[TILE_NULL] = {
		[DEEP] = 1,
		[WATER] = 1,
		[SAND] = 1,
		[GRASS] = 1,
		[TREE] = 1,
		[HILL] = 1,
	},
	[DEEP] = {
		[DEEP] = 2,
		[WATER] = 1,
	},
	[WATER] = {
		[DEEP] = 1,
		[WATER] = 1,
		[SAND] = 1,
	},
	[SAND] = {
		[WATER] = 1,
		[SAND] = 1,
		[GRASS] = 1,
	},
	[GRASS] = {
		[SAND] = 1,
		[GRASS] = 3,
		[TREE] = 1,
		[HILL] = 1,
	},
	[TREE] = {
		[TREE] = 1,
		[GRASS] = 1,
	},
	[HILL] = {
		[HILL] = 3,
		[GRASS] = 1,
	},
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

static void tile_apply_adjacency(int options[static TILE_COUNT], enum Tile adjacent) {
	for (int i = 0; i < TILE_COUNT; i++) {
		options[i] *= kTileWeightMatrix[adjacent][i];
	}
}

static int tile_option_sum(int options[static TILE_COUNT]) {
	int sum = 0;
	for (int i = 0; i < TILE_COUNT; i++) {
		sum += options[i];
	}
	return sum;
}

enum Tile tile_option_select(int options[static TILE_COUNT], int selection) {
	for (int i = 0; i < TILE_COUNT; i++) {
		if (selection < options[i]) {
			return (enum Tile) i;
		}
		selection -= options[i];
	}
	return TILE_NULL;
}

static void tile_generate(struct TileChunk* chunk, int row, int col) {
	int options[TILE_COUNT] = {
		[DEEP] = 1,
		[WATER] = 1,
		[SAND] = 1,
		[GRASS] = 1,
		[TREE] = 1,
		[HILL] = 1,
	};
	tile_apply_adjacency(options, chunk_get(chunk, row + 1, col));
	tile_apply_adjacency(options, chunk_get(chunk, row - 1, col));
	tile_apply_adjacency(options, chunk_get(chunk, row, col + 1));
	tile_apply_adjacency(options, chunk_get(chunk, row, col - 1));
	
	int sum = tile_option_sum(options);
	int selection = sum == 0 ? 0 : rand() % sum;
	
	// printf("Options: %d, selection %d\n", sum, selection);
	chunk->data[row * TILE_CHUNK_SIZE + col] = tile_option_select(options, selection);
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