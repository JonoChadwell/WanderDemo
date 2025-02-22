#pragma once

#include <stdint.h>
#include <array>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

constexpr int kChunkSize = 32;
constexpr int kChunkArraySize = kChunkSize * kChunkSize;

enum Tile {
	TILE_NULL = 0,
	DEEP,
	WATER,
	SAND,
	GRASS,
	TREE,
	HILL,
	TILE_COUNT,
};

struct Anchor {
	uint32_t seed;
};

struct TileChunk {
	struct Anchor anchor_ne;
	struct Anchor anchor_se;
	struct Anchor anchor_nw;
	struct Anchor anchor_sw;
	std::array<Tile, kChunkArraySize> data;
};


///////////////////////////////////////////////////////////////////////////////
// Chunk Functions
///////////////////////////////////////////////////////////////////////////////

// Returns TILE_NULL if row or col is out of bounds.
enum Tile chunk_get(struct TileChunk* chunk, int row, int col);

// Directionally generate new chunks.
void chunk_north(struct TileChunk* base, struct TileChunk* out);
void chunk_east(struct TileChunk* base, struct TileChunk* out);
void chunk_south(struct TileChunk* base, struct TileChunk* out);
void chunk_west(struct TileChunk* base, struct TileChunk* out);

// Build a root chunk.
void chunk_generate_root(struct TileChunk* chunk);
