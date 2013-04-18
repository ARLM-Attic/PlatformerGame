#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <string>
#include "util/vec2.hpp"

struct SpriteBuffer;

struct Map {
	int width, height;
	std::vector<uint16_t> data;

	Map()
		: width(0), height(0)
	{}

	uint16_t get(int x, int y) const {
		if (x < 0 || y < 0 || x >= width || y >= height) {
			return 0;
		}
		return data[y * width + x];
	}
};

Map loadMap(const std::string& filename);
void drawTilemap(
	const Map& map, SpriteBuffer& buffer,
	const int tile_w, const int tile_h, const int tiles_per_row,
	const ivec2 screen_pos,
	const int map_xoffset, const int map_yoffset,
	int map_wtiles, int map_htiles);
