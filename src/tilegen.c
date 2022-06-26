#include "tilegen.h"

#include <stdbool.h>
#include <stdlib.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Tilegen Constants
///////////////////////////////////////////////////////////////////////////////

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

int kRandomSet[TILE_COUNT] = {
	[DEEP] = 3,
	[WATER] = 2,
	[SAND] = 2,
	[GRASS] = 3,
	[TREE] = 2,
	[HILL] = 2,
};


///////////////////////////////////////////////////////////////////////////////
// RNG utils
///////////////////////////////////////////////////////////////////////////////

uint32_t scramble_north(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

uint32_t scramble_south(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x119de1f3u;
    x = ((x >> 16) ^ x) * 0x119de1f3u;
    x = (x >> 16) ^ x;
    return x;
}

uint32_t scramble_west(uint32_t x) {
    return x + 113u;
}

uint32_t scramble_east(uint32_t x) {
    return x - 113u;
}

uint32_t seed_blend(uint32_t a, uint32_t b) {
	return a * (b + 13337u);
}


///////////////////////////////////////////////////////////////////////////////
// Chunk Helper Functions
///////////////////////////////////////////////////////////////////////////////

static inline int ROW(int v) {
	return v / TILE_CHUNK_SIZE;
}

static inline int COL(int v) {
	return v % TILE_CHUNK_SIZE;
}

enum Tile chunk_get(struct TileChunk* chunk, int row, int col) {
	if (0 <= row && row < TILE_CHUNK_SIZE
	 && 0 <= col && col < TILE_CHUNK_SIZE) {
		return chunk->data[row * TILE_CHUNK_SIZE + col];
	}
	return TILE_NULL;
}

static void chunk_set(struct TileChunk* chunk, int row, int col, enum Tile value) {
	if (0 <= row && row < TILE_CHUNK_SIZE
	 && 0 <= col && col < TILE_CHUNK_SIZE) {
		chunk->data[row * TILE_CHUNK_SIZE + col] = value;
		
	}
}

static void chunk_clear(struct TileChunk* chunk) {
	for (int i = 0; i < TILE_CHUNK_ARRAY_SIZE; i++) {
		chunk->data[i] = TILE_NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Tile Generation
///////////////////////////////////////////////////////////////////////////////

static int tile_option_sum(int options[static TILE_COUNT]) {
	int sum = 0;
	for (int i = 0; i < TILE_COUNT; i++) {
		sum += options[i];
	}
	return sum;
}

static enum Tile tile_option_select(int options[static TILE_COUNT], uint32_t value) {
	int sum = tile_option_sum(options);
	int selection = sum == 0 ? 0 : (int) (value % sum);
	for (int i = 0; i < TILE_COUNT; i++) {
		if (selection < options[i]) {
			return (enum Tile) i;
		}
		selection -= options[i];
	}
	return TILE_NULL;
}

static enum Tile tile_option_random(int options[static TILE_COUNT]) {
	return tile_option_select(options, rand());
}

static void tile_apply_adjacency(int options[static TILE_COUNT], enum Tile adjacent) {
	for (int i = 0; i < TILE_COUNT; i++) {
		options[i] *= kTileWeightMatrix[adjacent][i];
	}
}

enum Tile tile_generate(struct TileChunk* chunk, int row, int col) {
	int options[TILE_COUNT] = { 0 };
	for (int i = 0; i < TILE_COUNT; ++i) {
		options[i] = kRandomSet[i];
	}
	tile_apply_adjacency(options, chunk_get(chunk, row + 1, col));
	tile_apply_adjacency(options, chunk_get(chunk, row - 1, col));
	tile_apply_adjacency(options, chunk_get(chunk, row, col + 1));
	tile_apply_adjacency(options, chunk_get(chunk, row, col - 1));
	
	bool difference = false;
	for (int i = 0; i < TILE_COUNT; ++i) {
		if (options[i] != kRandomSet[i]) {
			difference = true;
		}
	}
	if (!difference) {
		options[TILE_NULL] = 10;
	}
	
	return tile_option_random(options);
}

static bool tile_valid(struct TileChunk* chunk, int row, int col) {
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
	
	return options[chunk_get(chunk, row, col)] > 0;
}


///////////////////////////////////////////////////////////////////////////////
// Chunk creation tools
///////////////////////////////////////////////////////////////////////////////

static void create_corners(struct TileChunk* chunk) {
	chunk_set(chunk, 0, 0,
			tile_option_select(kRandomSet, chunk->anchor_nw.seed));
	chunk_set(chunk, 0, TILE_CHUNK_SIZE - 1,
			tile_option_select(kRandomSet, chunk->anchor_ne.seed));
	chunk_set(chunk, TILE_CHUNK_SIZE - 1, 0,
			tile_option_select(kRandomSet, chunk->anchor_sw.seed));
	chunk_set(chunk, TILE_CHUNK_SIZE - 1, TILE_CHUNK_SIZE - 1,
			tile_option_select(kRandomSet, chunk->anchor_se.seed));
}

// Create missing tiles in a line from the given start point to the end of the line.
//
// If `fix` is set, will attempt to fix invalid terrain.
//
// Returns true if all terrain in the line is valid.
static bool create_line(struct TileChunk* chunk, int start, int step, bool fix) {
	bool good = true;
	for (int i = 0; i < TILE_CHUNK_SIZE; i++) {
		int pos = start + i * step;
		
		if (pos < 0 || TILE_CHUNK_ARRAY_SIZE <= pos) {
			printf("Pos out of bounds\n");
			exit(EXIT_FAILURE);
		}
		
		if (chunk->data[pos] == TILE_NULL) {
			chunk->data[pos] = tile_generate(chunk, ROW(pos), COL(pos));
		}
		// See if removing the next tile in the line allows this tile to generate.
		if (fix && !tile_valid(chunk, ROW(pos), COL(pos))
				&& 0 <= pos + 2 * step && pos + 2 * step < TILE_CHUNK_ARRAY_SIZE) {
			enum Tile original_next = chunk->data[pos + step];
			chunk->data[pos + step] = TILE_NULL;
			chunk->data[pos] = tile_generate(chunk, ROW(pos), COL(pos));
			if (chunk->data[pos] == TILE_NULL) {
				chunk->data[pos + step] = original_next;
			}
		}
		if (chunk->data[pos] == TILE_NULL) {
			good = false;
		}
	}
	return good;
}

static void create_row(struct TileChunk* chunk, int row, bool backwards) {
	const int kMaxAttempts = 1000;
	for (int i = 0; i < kMaxAttempts; ++i) {
		if (create_line(chunk, row * TILE_CHUNK_SIZE, 1, false)) {
			break;
		}
		if (create_line(
				chunk,
				(row + 1) * TILE_CHUNK_SIZE - 1,
				-1,
				true)) {
			break;
		}
	}
}

static void create_col(struct TileChunk* chunk, int col, bool backwards) {
	const int kMaxAttempts = 1000;
	for (int i = 0; i < kMaxAttempts; ++i) {
		if (create_line(chunk, col, TILE_CHUNK_SIZE, false)) {
			break;
		}
		if (create_line(
				chunk,
				TILE_CHUNK_ARRAY_SIZE - TILE_CHUNK_SIZE + col,
				-TILE_CHUNK_SIZE,
				true)) {
			break;
		}
	}
}

static void create_edges(struct TileChunk* chunk) {
	srand(seed_blend(chunk->anchor_nw.seed, chunk->anchor_ne.seed));
	create_row(chunk, 0, false);
	srand(seed_blend(chunk->anchor_sw.seed, chunk->anchor_se.seed));
	create_row(chunk, TILE_CHUNK_SIZE - 1, false);
	srand(seed_blend(chunk->anchor_nw.seed, chunk->anchor_sw.seed));
	create_col(chunk, 0, false);
	srand(seed_blend(chunk->anchor_ne.seed, chunk->anchor_se.seed));
	create_col(chunk, TILE_CHUNK_SIZE - 1, false);
}

static void fix_row(struct TileChunk* chunk, int row) {
	create_line(chunk, row * TILE_CHUNK_SIZE, 1, true);
	create_line(chunk, row * TILE_CHUNK_SIZE + TILE_CHUNK_SIZE - 1, -1, true);
}

static void fix_col(struct TileChunk* chunk, int col) {
	create_line(chunk, col, TILE_CHUNK_SIZE, true);
	create_line(
			chunk,
			TILE_CHUNK_ARRAY_SIZE - TILE_CHUNK_SIZE + col,
			-TILE_CHUNK_SIZE,
			true);
}

///////////////////////////////////////////////////////////////////////////////
// Chunk Utils
///////////////////////////////////////////////////////////////////////////////

// NOT THREAD SAFE.
static int chunk_iterate(struct TileChunk* chunk, bool last) {
	static int invalid_list[TILE_CHUNK_ARRAY_SIZE] = { 0 };
	static enum Tile invalid_set_to[TILE_CHUNK_ARRAY_SIZE] = { 0 };
	int invalid_count = 0;
	
	for (int row = 1; row < TILE_CHUNK_SIZE - 1; row++) {
		for (int col = 1; col < TILE_CHUNK_SIZE - 1; col++) {
			if (!tile_valid(chunk, row, col)) {
				invalid_list[invalid_count++] = row * TILE_CHUNK_SIZE + col;
			}
		}
	}
	
	for (int i = 0; i < invalid_count; i++) {
		chunk->data[invalid_list[i]] = TILE_NULL;
	}
	
	if (last) {
		return invalid_count;
	}
	
	for (int i = 0; i < invalid_count; i++) {
		int row = ROW(invalid_list[i]);
		int col = COL(invalid_list[i]);
		invalid_set_to[i] = tile_generate(chunk, row, col);
	}
	
	for (int i = 0; i < invalid_count; i++) {
		chunk->data[invalid_list[i]] = invalid_set_to[i];
	}
	
	return invalid_count;
}

static void chunk_fix(struct TileChunk* chunk) {
	for (int i = 0; i < 3; i++) {
		for (int row = 0; row < TILE_CHUNK_SIZE; row++) {
			fix_row(chunk, row);
		}
		for (int col = 0; col < TILE_CHUNK_SIZE; col++) {
			fix_col(chunk, col);
		}
	}
}

static void chunk_generate(struct TileChunk* chunk) {
	static const int kIterations = 30;	
	static const int kMaxStrips = 3;

	uint32_t nw = chunk->anchor_nw.seed;
	uint32_t ne = chunk->anchor_ne.seed;
	uint32_t sw = chunk->anchor_sw.seed;
	uint32_t se = chunk->anchor_se.seed;

	// Start with the outside.
	chunk_clear(chunk);
	create_corners(chunk);
	create_edges(chunk);
	
	srand(seed_blend(seed_blend(nw, ne), seed_blend(sw, se)));
	
	// Generate some random strips.
	int num_rows = rand() % kMaxStrips;
	for (int i = 0; i < num_rows; ++i) {
		fix_row(chunk, rand() % (TILE_CHUNK_SIZE - 2) + 1);
	}
	int num_cols = rand() % kMaxStrips;
	for (int i = 0; i < num_cols; ++i) {
		fix_col(chunk, rand() % (TILE_CHUNK_SIZE - 2) + 1);
	}
	

	// Solve for the rest of the map.
	for (int i = kIterations; i > 0; --i) {
		int invalid = chunk_iterate(chunk, i == 1);
		if (invalid == 0) {
			break;
		}
	}
	
	chunk_fix(chunk);
}

void chunk_north(struct TileChunk* base, struct TileChunk* out) {
	out->anchor_ne = base->anchor_ne;
	out->anchor_se = base->anchor_ne;
	
	out->anchor_nw = base->anchor_nw;
	out->anchor_sw = base->anchor_nw;
	
	out->anchor_nw.seed = scramble_north(out->anchor_nw.seed);
	out->anchor_ne.seed = scramble_north(out->anchor_ne.seed);
	
	chunk_generate(out);
}

void chunk_east(struct TileChunk* base, struct TileChunk* out) {
	out->anchor_ne = base->anchor_ne;
	out->anchor_nw = base->anchor_ne;
	
	out->anchor_se = base->anchor_se;
	out->anchor_sw = base->anchor_se;
	
	out->anchor_se.seed = scramble_east(out->anchor_se.seed);
	out->anchor_ne.seed = scramble_east(out->anchor_ne.seed);
	
	chunk_generate(out);
}

void chunk_south(struct TileChunk* base, struct TileChunk* out) {
	out->anchor_ne = base->anchor_se;
	out->anchor_se = base->anchor_se;
	
	out->anchor_nw = base->anchor_sw;
	out->anchor_sw = base->anchor_sw;
	
	out->anchor_sw.seed = scramble_south(out->anchor_sw.seed);
	out->anchor_se.seed = scramble_south(out->anchor_se.seed);
	
	chunk_generate(out);
}

void chunk_west(struct TileChunk* base, struct TileChunk* out) {
	out->anchor_ne = base->anchor_nw;
	out->anchor_nw = base->anchor_nw;
	
	out->anchor_se = base->anchor_sw;
	out->anchor_sw = base->anchor_sw;
	
	out->anchor_nw.seed = scramble_west(out->anchor_nw.seed);
	out->anchor_sw.seed = scramble_west(out->anchor_sw.seed);
	
	chunk_generate(out);
}

void chunk_generate_root(struct TileChunk* chunk) {
	static const int kSeed = 2;

	chunk->anchor_nw.seed = kSeed;
	chunk->anchor_ne.seed = scramble_east(kSeed);
	chunk->anchor_sw.seed = scramble_south(kSeed);
	chunk->anchor_se.seed = scramble_east(scramble_south(kSeed));

	chunk_generate(chunk);
}
